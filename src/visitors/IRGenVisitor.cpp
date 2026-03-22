#include "visitors/IRGenVisitor.h"

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

namespace thogcc::visitors {

int IRGenVisitor::_emitInstr(const ir::LLVMInstruction& instr) {
    const int id = (int)_function->instructions.size();
    _function->instructions.push_back(instr);
    _function->blocks.back().instrIDs.push_back({.id = id});
    return id;
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
    for (int i = _identifiers.size() - 1; i >= 0; i--) {
        if (_identifiers.at(i).contains(name)) return _identifiers.at(i).at(name);
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
        .type = {ir::LLVMBasicType::VOID, 0},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::PARAM,
                    .id = (int)this->_function->params.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)this->_function->instructions.size() - 1,
                },
            },
    });
}

void IRGenVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    _function->name = node.getIdentifier();
    if (node.getParams() != nullptr) node.getParams()->accept(*this);
    if (node.getIdentifiers() != nullptr) node.getIdentifiers()->accept(*this);
}

void IRGenVisitor::visit(ast::declarators::InitDeclarator& node) {
    node.getDecl()->accept(*this);
    if (_function == nullptr) {
        _global->type = types::toLLVMType(
            *std::get<std::shared_ptr<types::VarSymbol>>(node.getSymbol()).get()->type);
        if (node.getInitializer() != nullptr) node.getInitializer()->accept(*this);
    } else {
        *_type = types::toLLVMType(
            *std::get<std::shared_ptr<types::VarSymbol>>(node.getSymbol()).get()->type);

        _initialising = node.getIdentifier();
        if (node.getInitializer() != nullptr) node.getInitializer()->accept(*this);
    }
}

void IRGenVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    if (_global != nullptr) {
        // identifier for global so yay
        _global->name = node.getIdentifier();
        _currentIdentifiers()[node.getIdentifier()] = {
            ir::LLVMValueKind::GLOBAL,
            static_cast<int>(_module.globals.size()),
        };
    } else {
        const ir::LLVMInstruction instr = {
            .opcode = ir::LLVMOpcode::ALLOCA,
            .type =
                types::toLLVMType(*std::visit([](auto& v) { return v->type; }, node.getSymbol())),
        };
        _emitInstr(instr);
        _currentIdentifiers()[node.getIdentifier()] = {
            ir::LLVMValueKind::INSTR, static_cast<int>(_function->instructions.size() - 1)};
    }
    // TODO
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
    int ifID = _function->instructions.size();

    const ir::LLVMInstruction instr = {
        .opcode = ir::LLVMOpcode::BR,
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)_function->instructions.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::BLOCK,
                    .id = (int)_function->blocks.size(),
                },
                {
                    .kind = ir::LLVMValueKind::BLOCK,
                    .id = 0,
                },
            },
    };

    const int index = _function->instructions.size();

    _emitInstr(instr);

    const ir::LLVMInstruction jumpEnd = {
        .opcode = ir::LLVMOpcode::BR,
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::BLOCK,
                    .id = 0,
                },
            },
    };

    const int startJumpEnd = _function->instructions.size();
    _function->instructions.push_back(jumpEnd);

    _createBlock(std::format("if_true_{}", ifID));

    node.getIfStatement()->accept(*this);

    const int endIndex = _function->instructions.size();
    _emitInstr(jumpEnd);

    if (node.getElseStatement() != nullptr) {
        _createBlock(std::format("if_false_{}", ifID));

        _function->instructions[index].operands[2].id = _function->blocks.size() - 1;

        node.getElseStatement()->accept(*this);
    }

    _createBlock(std::format("if_end_{}", ifID));

    if (_function->instructions[index].operands[2].id == 0) {
        _function->instructions[index].operands[2].id = _function->blocks.size() - 1;
    }
    _function->instructions[endIndex].operands[0].id = _function->blocks.size() - 1;
    _function->instructions[startJumpEnd].operands[0].id = _function->blocks.size() - 1;
}

void IRGenVisitor::visit(ast::expressions::binary::AddMultExpression& node) {
    node.getRhs()->accept(*this);
    int rhs = _function->instructions.size() - 1;
    if (_function->instructions.at(rhs).type.type == ir::LLVMBasicType::PTR) {
        rhs = _emitInstr({
            .opcode = ir::LLVMOpcode::LOAD,
            .type = types::toLLVMType(*node.getRhs()->getEvaluatedType()),
            .operands = {{.kind = ir::LLVMValueKind::INSTR, .id = rhs}},
            .cond{},
        });
    }

    node.getLhs()->accept(*this);
    int lhs = _function->instructions.size() - 1;
    if (_function->instructions.at(lhs).type.type == ir::LLVMBasicType::PTR) {
        lhs = _emitInstr({
            .opcode = ir::LLVMOpcode::LOAD,
            .type = types::toLLVMType(*node.getLhs()->getEvaluatedType()),
            .operands = {{.kind = ir::LLVMValueKind::INSTR, .id = lhs}},
            .cond{},
        });
    }

    ir::LLVMOpcode op;

    switch (node.getOp()) {
        case ast::expressions::binary::AddMultExpressionType::ADD:
            op = ir::LLVMOpcode::ADD;
            break;
        case ast::expressions::binary::AddMultExpressionType::SUB:
            op = ir::LLVMOpcode::SUB;
            break;
        case ast::expressions::binary::AddMultExpressionType::MUL:
            op = ir::LLVMOpcode::MUL;
            break;
        case ast::expressions::binary::AddMultExpressionType::DIV:
            op = ir::LLVMOpcode::SDIV;
            break;
        case ast::expressions::binary::AddMultExpressionType::REM:
            op = ir::LLVMOpcode::SREM;
            break;
    }

    _emitInstr({
        .opcode = op,
        .type = types::toLLVMType(*node.getRhs()->getEvaluatedType()),
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)_function->instructions.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = rhs,
                },
            },
    });
}

void IRGenVisitor::visit(ast::expressions::binary::EqualityExpression& node) {
    node.getRhs()->accept(*this);
    const int rhs = _function->instructions.size() - 1;
    node.getLhs()->accept(*this);
    const int lhs = _function->instructions.size() - 1;

    ir::LLVMInstruction instr;
    if (node.getLhs()->isLvalue()) {
        instr = {
            .opcode = ir::LLVMOpcode::LOAD,
            .type = types::toLLVMType(*node.getRhs()->getEvaluatedType()),
            .operands =
                {
                    {
                        .kind = ir::LLVMValueKind::INSTR,
                        .id = lhs,
                    },
                },
        };
        _emitInstr(instr);
    }

    instr = {
        .opcode = ir::LLVMOpcode::ICMP,
        .type =
            {
                .type = ir::LLVMBasicType::INT,
                .intSize = 1,
            },
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)_function->instructions.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = rhs,
                },
            },
        .cond = node.getIsNe() ? ir::LLVMCmpCond::NE : ir::LLVMCmpCond::EQ,
    };
    _emitInstr(instr);
}

void IRGenVisitor::visit(ast::statements::ReturnStatement& node) {
    node.getExpr()->accept(*this);
    const int ret = _function->instructions.size() - 1;

    ir::LLVMInstruction instr;
    if (node.getExpr()->isLvalue()) {
        instr = {
            .opcode = ir::LLVMOpcode::LOAD,
            .type = _function->instructions.at(ret).type,
            .operands =
                {
                    {
                        .kind = ir::LLVMValueKind::INSTR,
                        .id = ret,
                    },
                },
        };
        _emitInstr(instr);
    }

    instr = {
        .opcode = ir::LLVMOpcode::RET,
        .type = types::toLLVMType(*node.getExpr()->getEvaluatedType()),
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)_function->instructions.size() - 1,
                },
            },
    };
    _emitInstr(instr);
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

    const ir::LLVMInstruction instr = ir::LLVMInstruction{
        .opcode = ir::LLVMOpcode::STORE,
        .type = {ir::LLVMBasicType::VOID, 0},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)_function->instructions.size() - 1,
                },
                _resolveIdentifier(_initialising),
            },
    };
    _emitInstr(instr);
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
                            .id = (int)_function->consts.size() - 1,
                        },
                        {
                            .kind = ir::LLVMValueKind::CONST,
                            .id = 0,
                        },
                    },
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
            //                     .id = (int)_function->instructions.size() - 2,
            //                 },
            //                 {
            //                     .kind = ir::LLVMValueKind::INSTR,
            //                     .id = (int)_function->instructions.size() - 1,
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

void IRGenVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    ir::LLVMInstruction* temp = _instruction;

    node.getRhs()->accept(*this);
    const int rhs = _function->instructions.size() - 1;

    node.getLhs()->accept(*this);
    const int lhs = _function->instructions.size() - 1;

    ir::LLVMInstruction instr;

    // ASSUMES LHS ALWAYS RETURNS A POINTER, RHS ALWAYS RETURNS A VALUE
    // TODO:
    // once typing things exist this can be fixed to respect the lhs type correctly,
    // currently always assumes lhs dereferenced is the same type as the rhs for loading
    switch (node.getOp()) {
        case ast::expressions::binary::AssignmentExpressionType::MUL_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            _emitInstr(instr);
            const int loaded = _function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::MUL,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = loaded,
                        },
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = rhs,
                        },
                    },
            };
            _emitInstr(instr);
            break;
        }
        // TODO: fix with signed/unsigned
        case ast::expressions::binary::AssignmentExpressionType::DIV_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            _emitInstr(instr);
            const int loaded = _function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::UDIV,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = loaded,
                        },
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = rhs,
                        },
                    },
            };
            _emitInstr(instr);
            break;
        }
            // TODO: same as above
        case ast::expressions::binary::AssignmentExpressionType::MOD_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            _emitInstr(instr);
            const int loaded = _function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::UREM,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = loaded,
                        },
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = rhs,
                        },
                    },
            };
            _emitInstr(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::ADD_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };

            _emitInstr(instr);
            const int loaded = _function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::ADD,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = loaded,
                        },
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = rhs,
                        },
                    },
            };
            _emitInstr(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::SUB_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            _emitInstr(instr);
            const int loaded = _function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::SUB,
                .type = _function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = loaded,
                        },
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = rhs,
                        },
                    },
            };
            _emitInstr(instr);
            break;
        }
        // do these after signed things exist
        case ast::expressions::binary::AssignmentExpressionType::LEFT_ASSIGN:
        case ast::expressions::binary::AssignmentExpressionType::RIGHT_ASSIGN:
        case ast::expressions::binary::AssignmentExpressionType::AND_ASSIGN:
        case ast::expressions::binary::AssignmentExpressionType::XOR_ASSIGN:
        case ast::expressions::binary::AssignmentExpressionType::OR_ASSIGN:
        case ast::expressions::binary::AssignmentExpressionType::ASSIGN:
        default: {
            break;
        }
    }

    _emitInstr({
        .opcode = ir::LLVMOpcode::STORE,
        .type = {ir::LLVMBasicType::VOID, 0},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = rhs,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = lhs,
                },
            },
    });

    _instruction = temp;
}

}  // namespace thogcc::visitors
