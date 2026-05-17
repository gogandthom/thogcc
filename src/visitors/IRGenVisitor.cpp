#include "visitors/IRGenVisitor.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include "ast/Node.h"
#include "ast/all.h"
#include "ir/LLVMModule.h"
#include "ir/LLVMType.h"
#include "types/Scope.h"
#include "types/helpers.h"
#include "utils.h"

// NOLINTBEGIN(*-magic-numbers)

namespace thogcc::visitors {

std::size_t IRGenVisitor::_emitInstr(const ir::LLVMInstruction& instr) {
    const std::size_t id = _function->instructions.size();
    _function->instructions.push_back(instr);
    _function->blocks.back().instrIDs.push_back({.id = id});
    return id;
}

std::size_t IRGenVisitor::_evaluateAsRValue(ast::expressions::ExpressionBase& node) {
    node.accept(*this);
    std::size_t instrID = _function->instructions.size() - 1;
    if (_function->instructions.at(instrID).type.type == ir::LLVMBasicType::PTR) {
        instrID = _emitInstr({
            .opcode = ir::LLVMOpcode::LOAD,
            .type = types::toLLVMType(*node.getEvaluatedType()),
            .operands = {{.kind = ir::LLVMValueKind::INSTR, .id = instrID}},
            .cond{},
        });
    }
    return instrID;
}

ir::LLVMBasicBlock& IRGenVisitor::_createBlock(std::string label) {
    return _function->blocks.emplace_back(ir::LLVMBasicBlock{
        .label = std::move(label),
        .instrIDs = {},
    });
}

IRGenVisitor::IRGenVisitor(ir::LLVMModule& module) : _module(module) {}

void IRGenVisitor::visit(ast::Node& /*node*/) {}

void IRGenVisitor::visit(ast::NodeListBase& node) {
    for (size_t i = 0; i < node.size(); i++) {
        node.getRawNode(i).accept(*this);
    }
}

std::map<std::string_view, ir::LLVMValueID>& IRGenVisitor::_currentIdentifiers() {
    if (_identifiers.empty()) _identifiers.push_back({});
    return _identifiers.at(_identifiers.size() - 1);
}

ir::LLVMValueID IRGenVisitor::_resolveIdentifier(std::string_view name) {
    for (const auto& ident : _identifiers) {
        if (ident.contains(name)) return ident.at(name);
    }
    throw std::runtime_error(std::format("Unknown identifier {}", name));
}

void IRGenVisitor::visit(ast::declarations::Declaration& node) {
    if (_function == nullptr) {
        // global declaration
        ir::LLVMGlobal global = {};
        _global = &global;
        // node.getSpecifiers()->accept(*this);
        node.getDeclarators()->accept(*this);
        _global = nullptr;
        _module.globals.push_back(global);
        // _currentIdentifiers().at(_global->name)
    } else {
        // local declaration
        ir::LLVMType type = {};
        _type = &type;
        // node.getSpecifiers()->accept(*this);
        node.getDeclarators()->accept(*this);
        _type = nullptr;
        // _currentIdentifiers().at(this)
    }
}

void IRGenVisitor::visit(ast::declarations::FunctionDefinition& node) {
    ir::LLVMFunction func = {};
    _function = &func;

    func.returnType =
        types::toLLVMType(*std::get<types::FuncType>(node.getSymbol()->type->data).returnType);

    _createBlock("");

    func.consts.push_back({
        .type =
            {
                .type = ir::LLVMBasicType::INT,
                .intSize = 32,
            },
        .value = (uint64_t)0,
    });

    _identifiers.push_back({});
    // node.getSpecifiers()->accept(*this);
    node.getDeclarator()->accept(*this);
    if (node.getDeclarations() != nullptr) node.getDeclarations()->accept(*this);
    if (node.getStatement() != nullptr) node.getStatement()->accept(*this);
    _function = nullptr;
    _module.functions.push_back(func);
    _identifiers.pop_back();
}

void IRGenVisitor::visit(ast::declarations::ParameterDeclaration& node) {
    node.getDecl()->accept(*this);

    this->_function->params.push_back(ir::LLVMParameter{
        // TODO
        .type = {types::toLLVMType(*node.getSymbol()->type)},
        .name = (std::string)node.getDecl()->getIdentifier(),
    });

    _emitInstr({
        .opcode = ir::LLVMOpcode::STORE,
        .type{},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::PARAM,
                    .id = this->_function->params.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = this->_function->instructions.size() - 1,
                },
            },
        .cond{},
    });
}

void IRGenVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    _function->name = node.getIdentifier();
    if (node.getParams() != nullptr) node.getParams()->accept(*this);
    if (node.getIdentifiers() != nullptr) node.getIdentifiers()->accept(*this);
}

void IRGenVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    if (_global != nullptr) {
        // identifier for global so yay
        _global->name = node.getIdentifier();
        _currentIdentifiers()[node.getIdentifier()] = {
            ir::LLVMValueKind::GLOBAL,
            _module.globals.size(),
        };
    } else {
        _emitInstr({
            .opcode = ir::LLVMOpcode::ALLOCA,
            .type =
                types::toLLVMType(*std::visit([](auto& v) { return v->type; }, node.getSymbol())),
            .operands{},
            .cond{},
        });
        _currentIdentifiers()[node.getIdentifier()] = {
            ir::LLVMValueKind::INSTR,
            _function->instructions.size() - 1,
        };
    }
    // TODO
}

void IRGenVisitor::visit(ast::declarators::InitDeclarator& node) {
    node.getDecl()->accept(*this);
    if (_function == nullptr) {
        _global->type =
            types::toLLVMType(*std::get<std::shared_ptr<types::VarSymbol>>(node.getSymbol())->type);
        if (node.getInitializer() != nullptr) node.getInitializer()->accept(*this);
    } else {
        *_type =
            types::toLLVMType(*std::get<std::shared_ptr<types::VarSymbol>>(node.getSymbol())->type);

        _initialising = node.getIdentifier();
        if (node.getInitializer() != nullptr) node.getInitializer()->accept(*this);
    }
}

void IRGenVisitor::visit(ast::expressions::IdentifierExpression& node) {
    if (node.isLvalue()) {
        _emitInstr(ir::LLVMInstruction{
            .opcode = ir::LLVMOpcode::ADD,
            .type = {.type = ir::LLVMBasicType::PTR, .intSize = 0},
            .operands =
                {
                    _resolveIdentifier(node.getIdentifier()),
                    {
                        .kind = ir::LLVMValueKind::CONST,
                        .id = 0,
                    },
                },
            .cond{},
        });
    } else {
        _emitInstr(ir::LLVMInstruction{
            .opcode = ir::LLVMOpcode::LOAD,
            .type = types::toLLVMType(*node.getEvaluatedType()),
            .operands =
                {
                    _resolveIdentifier(node.getIdentifier()),
                },
            .cond{},
        });
    }
}

void IRGenVisitor::visit(ast::expressions::Initializer& node) {
    std::visit(
        overload{
            [this](const std::unique_ptr<ast::expressions::ExpressionBase>& expr) {
                expr->accept(*this);
            },
            [this](const std::unique_ptr<ast::NodeList<ast::expressions::Initializer>>& nodeList) {
                nodeList->accept(*this);
            }},
        node.getChild());

    _emitInstr({
        .opcode = ir::LLVMOpcode::STORE,
        .type{},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = _function->instructions.size() - 1,
                },
                _resolveIdentifier(_initialising),
            },
        .cond{},
    });
}

void IRGenVisitor::visit(ast::expressions::ListExpression& node) {
    for (size_t i = 0; i < node.getList()->size(); i++) {
        node.getList()->getRawNode(i).accept(*this);
    }
}

void IRGenVisitor::visit(ast::expressions::PrimaryExpression& node) {
    auto kind = std::get<types::BasicType>(node.getEvaluatedType()->data).kind;

    bool doesnTExist = false;

    ir::LLVMInstruction instr = {};

    switch (kind) {
        case types::BasicType::Kind::INT:
            _function->consts.push_back({
                .type =
                    {
                        .type = ir::LLVMBasicType::INT,
                        .intSize = 32,
                    },
                .value = (uint64_t)std::get<int>(node.getValue()),
            });
            instr = ir::LLVMInstruction{
                .opcode = ir::LLVMOpcode::ADD,
                .type =
                    {
                        .type = ir::LLVMBasicType::INT,
                        .intSize = 32,
                    },
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::CONST,
                            .id = _function->consts.size() - 1,
                        },
                        {
                            .kind = ir::LLVMValueKind::CONST,
                            .id = 0,
                        },
                    },
                .cond{},
            };
            break;
        case types::BasicType::Kind::VOID:
        default:
            doesnTExist = true;
            break;
    }

    if (!doesnTExist) {
        if (_global == nullptr) {
            _emitInstr(instr);

            // if (node.isLvalue()) {
            //     instr = ir::LLVMInstruction{
            //         .opcode = ir::LLVMOpcode::ALLOCA,
            //         .type = instr.type,
            //     };
            //     _emitInstr(instr);
            //     instr = ir::LLVMInstruction{
            //         .opcode = ir::LLVMOpcode::STORE,
            //         .operands =
            //             {
            //                 {
            //                     .kind = ir::LLVMValueKind::INSTR,
            //                     .id = _function->instructions.size() - 2,
            //                 },
            //                 {
            //                     .kind = ir::LLVMValueKind::INSTR,
            //                     .id = _function->instructions.size() - 1,
            //                 },
            //             },
            //     };
            //     _emitInstr(instr);
            // }
        } else {
            // die
        }
    }
}

void IRGenVisitor::visit(ast::expressions::binary::AddMultExpression& node) {
    const std::size_t rhs = _evaluateAsRValue(*node.getRhs());
    const std::size_t lhs = _evaluateAsRValue(*node.getLhs());

    auto toOp = [](ast::expressions::binary::AddMultExpressionType type) -> ir::LLVMOpcode {
        switch (type) {
            case ast::expressions::binary::AddMultExpressionType::ADD:
                return ir::LLVMOpcode::ADD;
            case ast::expressions::binary::AddMultExpressionType::SUB:
                return ir::LLVMOpcode::SUB;
            case ast::expressions::binary::AddMultExpressionType::MUL:
                return ir::LLVMOpcode::MUL;
            case ast::expressions::binary::AddMultExpressionType::DIV:
                return ir::LLVMOpcode::SDIV;
            case ast::expressions::binary::AddMultExpressionType::REM:
                return ir::LLVMOpcode::SREM;
        }
        assert(false && "Unhandled AddMultExpressionType");
        __builtin_unreachable();
    };

    _emitInstr({
        .opcode = toOp(node.getOp()),
        .type = types::toLLVMType(*node.getRhs()->getEvaluatedType()),
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = lhs,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = rhs,
                },
            },
        .cond{},
    });
}

void IRGenVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    ir::LLVMInstruction* temp = _instruction;

    const std::size_t rhs = _evaluateAsRValue(*node.getRhs());

    // ASSUMES LHS ALWAYS RETURNS A POINTER
    node.getLhs()->accept(*this);
    const std::size_t lhs = _function->instructions.size() - 1;

    // TODO: fix with signed/unsigned
    auto toOp = [](ast::expressions::binary::AssignmentExpressionType type) -> ir::LLVMOpcode {
        switch (type) {
            case ast::expressions::binary::AssignmentExpressionType::ASSIGN:
                break;  // Not a compound assignment
            case ast::expressions::binary::AssignmentExpressionType::MUL_ASSIGN:
                return ir::LLVMOpcode::MUL;
            case ast::expressions::binary::AssignmentExpressionType::DIV_ASSIGN:
                return ir::LLVMOpcode::UDIV;
            case ast::expressions::binary::AssignmentExpressionType::MOD_ASSIGN:
                return ir::LLVMOpcode::UREM;
            case ast::expressions::binary::AssignmentExpressionType::ADD_ASSIGN:
                return ir::LLVMOpcode::ADD;
            case ast::expressions::binary::AssignmentExpressionType::SUB_ASSIGN:
                return ir::LLVMOpcode::SUB;
            case ast::expressions::binary::AssignmentExpressionType::LEFT_ASSIGN:
            case ast::expressions::binary::AssignmentExpressionType::RIGHT_ASSIGN:
            case ast::expressions::binary::AssignmentExpressionType::AND_ASSIGN:
                return ir::LLVMOpcode::AND;
            case ast::expressions::binary::AssignmentExpressionType::XOR_ASSIGN:
                return ir::LLVMOpcode::XOR;
            case ast::expressions::binary::AssignmentExpressionType::OR_ASSIGN:
                return ir::LLVMOpcode::OR;
        }
        assert(false && "Unhandled AssignmentExpressionType");
        __builtin_unreachable();
    };

    const ir::LLVMType type = _function->instructions.at(rhs).type;

    // compound assignment
    // TODO currently always assumes lhs dereferenced is the same type as the rhs for loading
    if (node.getOp() != ast::expressions::binary::AssignmentExpressionType::ASSIGN) {
        const std::size_t loadedLhs = _emitInstr({
            .opcode = ir::LLVMOpcode::LOAD,
            .type = type,
            .operands = {{.kind = ir::LLVMValueKind::INSTR, .id = lhs}},
            .cond{},
        });
        const ir::LLVMOpcode op = toOp(node.getOp());
        _emitInstr({
            .opcode = op,
            .type = type,
            .operands =
                {
                    {.kind = ir::LLVMValueKind::INSTR, .id = loadedLhs},
                    {.kind = ir::LLVMValueKind::INSTR, .id = rhs},
                },
            .cond{},
        });
    }

    _emitInstr({
        .opcode = ir::LLVMOpcode::STORE,
        .type{},
        .operands =
            {
                {.kind = ir::LLVMValueKind::INSTR, .id = rhs},
                {.kind = ir::LLVMValueKind::INSTR, .id = lhs},
            },
        .cond{},
    });

    _instruction = temp;
}

void IRGenVisitor::visit(ast::expressions::binary::BitwiseExpression& node) {
    const std::size_t rhs = _evaluateAsRValue(*node.getRhs());
    const std::size_t lhs = _evaluateAsRValue(*node.getLhs());

    auto toOp = [](ast::expressions::binary::BitwiseExpressionType type) -> ir::LLVMOpcode {
        switch (type) {
            case ast::expressions::binary::BitwiseExpressionType::AND:
                return ir::LLVMOpcode::AND;
            case ast::expressions::binary::BitwiseExpressionType::OR:
                return ir::LLVMOpcode::OR;
            case ast::expressions::binary::BitwiseExpressionType::XOR:
                return ir::LLVMOpcode::XOR;
        }
        assert(false && "Unhandled BitwiseExpressionType");
        __builtin_unreachable();
    };

    _emitInstr({
        .opcode = toOp(node.getOp()),
        .type = types::toLLVMType(*node.getRhs()->getEvaluatedType()),
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = lhs,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = rhs,
                },
            },
        .cond{},
    });
}

void IRGenVisitor::visit(ast::expressions::binary::EqualityExpression& node) {
    const std::size_t rhs = _evaluateAsRValue(*node.getRhs());
    const std::size_t lhs = _evaluateAsRValue(*node.getLhs());

    _emitInstr({
        .opcode = ir::LLVMOpcode::ICMP,
        .type =
            {
                .type = ir::LLVMBasicType::INT,
                .intSize = 1,
            },
        .operands =
            {
                {.kind = ir::LLVMValueKind::INSTR, .id = lhs},
                {.kind = ir::LLVMValueKind::INSTR, .id = rhs},
            },
        .cond = node.getIsNe() ? ir::LLVMCmpCond::NE : ir::LLVMCmpCond::EQ,
    });
}

void IRGenVisitor::visit(ast::statements::CompoundStatement& node) {
    if (node.getDeclarationList() != nullptr) node.getDeclarationList()->accept(*this);
    if (node.getStatementList() != nullptr) node.getStatementList()->accept(*this);
}

void IRGenVisitor::visit(ast::statements::ExpressionStatement& node) {
    node.getExpr()->accept(*this);
}

void IRGenVisitor::visit(ast::statements::IfStatement& node) {
    node.getCond()->accept(*this);
    const std::size_t ifID = _function->instructions.size();

    _emitInstr({
        .opcode = ir::LLVMOpcode::BR,
        .type{},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = _function->instructions.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::BLOCK,
                    .id = _function->blocks.size(),
                },
                {
                    .kind = ir::LLVMValueKind::BLOCK,
                    .id = 0,
                },
            },
        .cond{},
    });

    const ir::LLVMInstruction jumpEnd = {
        .opcode = ir::LLVMOpcode::BR,
        .type{},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::BLOCK,
                    .id = 0,
                },
            },
        .cond{},
    };

    const std::size_t startJumpEnd = _function->instructions.size();
    _function->instructions.push_back(jumpEnd);

    _createBlock(std::format("if_true_{}", ifID));

    node.getIfStatement()->accept(*this);

    const std::size_t endIndex = _function->instructions.size();
    _emitInstr(jumpEnd);

    if (node.getElseStatement() != nullptr) {
        _createBlock(std::format("if_false_{}", ifID));

        _function->instructions[ifID].operands[2].id = _function->blocks.size() - 1;

        node.getElseStatement()->accept(*this);
    }

    _createBlock(std::format("if_end_{}", ifID));

    if (_function->instructions[ifID].operands[2].id == 0) {
        _function->instructions[ifID].operands[2].id = _function->blocks.size() - 1;
    }
    _function->instructions[endIndex].operands[0].id = _function->blocks.size() - 1;
    _function->instructions[startJumpEnd].operands[0].id = _function->blocks.size() - 1;
}

void IRGenVisitor::visit(ast::statements::ReturnStatement& node) {
    if (node.getExpr() != nullptr) {
        // _expr is a ListExpression. This checks whether the last emitted instruction is a pointer.
        // TODO Is this correct, or should we be checking whether the last Expression is an lvalue?
        const std::size_t ret = _evaluateAsRValue(*node.getExpr());
        _emitInstr({
            .opcode = ir::LLVMOpcode::RET,
            .type = types::toLLVMType(*node.getExpr()->getEvaluatedType()),
            .operands = {{.kind = ir::LLVMValueKind::INSTR, .id = ret}},
            .cond{},
        });
    } else {
        // void return
        _emitInstr({
            .opcode = ir::LLVMOpcode::RET,
            .type = {.type = ir::LLVMBasicType::VOID},
            .operands = {},
            .cond{},
        });
    }
}

}  // namespace thogcc::visitors

// NOLINTEND(*-magic-numbers)
