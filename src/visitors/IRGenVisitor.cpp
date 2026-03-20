#include "visitors/IRGenVisitor.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include "ast/Node.h"
#include "ast/all.h"
#include "ir/LLVMType.h"
#include "ir/llvm.h"
#include "types/helpers.h"
#include "utils.h"

namespace thogcc::visitors {

IRGenVisitor::IRGenVisitor(std::string srcFilePath) : _function(nullptr) {
    this->_module = {};
    this->_module.srcFileName = std::move(srcFilePath);

    this->_identifiers.push_back({});
}

ir::LLVMModule IRGenVisitor::getModule() {
    return this->_module;
}

void IRGenVisitor::visit(ast::Node& node) {
    std::cout << "no " << ast::nodeKindName(node.getKind()) << std::endl;
}

void IRGenVisitor::visit(ast::NodeListBase& node) {
    for (size_t i = 0; i < node.size(); i++) {
        node.getRawNode(i).accept(*this);
    }
}

std::map<std::string_view, int>& IRGenVisitor::_currentIdentifiers() {
    if (this->_identifiers.size() == 0) this->_identifiers.push_back({});
    return this->_identifiers.at(this->_identifiers.size() - 1);
}

int IRGenVisitor::_resolveIdentifier(std::string_view name) {
    for (int i = this->_identifiers.size() - 1; i >= 0; i--) {
        if (this->_identifiers.at(i).contains(name)) return this->_identifiers.at(i).at(name);
    }
}

void IRGenVisitor::visit(ast::declarations::Declaration& node) {
    if (this->_function == nullptr) {
        // global declaration
        ir::LLVMGlobal global = {};
        this->_global = &global;
        // node.getSpecifiers()->accept(*this);
        node.getDeclarators()->accept(*this);
        this->_global = nullptr;
        this->_module.globals.push_back(global);
        // this->_currentIdentifiers().at(this->_global->name)
    } else {
        // local declaration
        ir::LLVMType type = {};
        this->_type = &type;
        // node.getSpecifiers()->accept(*this);
        node.getDeclarators()->accept(*this);
        this->_type = nullptr;
        // this->_currentIdentifiers().at(this)
    }
}

void IRGenVisitor::visit(ast::declarations::FunctionDefinition& node) {
    ir::LLVMFunction func = {};

    func.returnType =
        types::toLLVMType(*std::get<types::FuncType>(node.getSymbol()->type->data).returnType);

    func.blocks.push_back({
        .label = "",
        .instrIDs = {},
    });
    func.consts.push_back({
        .type =
            {
                .type = ir::LLVMBasicType::INT,
                .intSize = 32,
            },
        .value = (uint64_t)0,
    });

    this->_identifiers.push_back({});

    this->_function = &func;
    // node.getSpecifiers()->accept(*this);
    node.getDeclarator()->accept(*this);
    if (node.getDeclarations() != nullptr) node.getDeclarations()->accept(*this);
    if (node.getStatement() != nullptr) node.getStatement()->accept(*this);
    this->_function = nullptr;
    this->_module.functions.push_back(func);
    this->_identifiers.pop_back();
}

void IRGenVisitor::visit(ast::declarations::ParameterDeclaration& node) {
    this->_function->params.push_back(ir::LLVMParameter{
        // TODO
        .type = {types::toLLVMType(*node.getSymbol()->type)},
        .name = (std::string)node.getDecl()->getIdentifier(),
    });
}

void IRGenVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    this->_function->name = node.getIdentifier();
    if (node.getParams() != nullptr) node.getParams()->accept(*this);
    if (node.getIdentifiers() != nullptr) node.getIdentifiers()->accept(*this);
}

void IRGenVisitor::visit(ast::declarators::InitDeclarator& node) {
    node.getDecl()->accept(*this);
    if (!this->_function) {
        this->_global->type = types::toLLVMType(
            *std::get<std::shared_ptr<types::VarSymbol>>(node.getSymbol()).get()->type);
        if (node.getInitializer()) node.getInitializer()->accept(*this);
    } else {
        *this->_type = types::toLLVMType(
            *std::get<std::shared_ptr<types::VarSymbol>>(node.getSymbol()).get()->type);

        this->_initialising = node.getIdentifier();
        if (node.getInitializer()) node.getInitializer()->accept(*this);
    }
}

void IRGenVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    if (this->_global != nullptr) {
        // identifier for global so yay
        this->_global->name = node.getIdentifier();
    } else {
        ir::LLVMInstruction instr = {
            .opcode = ir::LLVMOpcode::ALLOCA,
            .type =
                types::toLLVMType(*std::visit([](auto& v) { return v->type; }, node.getSymbol())),
        };
        this->_function->blocks.at(this->_function->blocks.size() - 1)
            .instrIDs.push_back({
                .id = (int)this->_function->instructions.size(),
            });
        this->_function->instructions.push_back(instr);
        this->_currentIdentifiers()[node.getIdentifier()] =
            this->_function->instructions.size() - 1;
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

void IRGenVisitor::visit(ast::statements::ReturnStatement& node) {
    node.getExpr()->accept(*this);
    const int ret = this->_function->instructions.size() - 1;

    ir::LLVMInstruction instr = {
        .opcode = ir::LLVMOpcode::LOAD,
        .type = this->_function->instructions.at(ret).type,
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = ret,
                },
            },
    };
    this->_function->blocks.at(this->_function->blocks.size() - 1)
        .instrIDs.push_back({
            .id = (int)this->_function->instructions.size(),
        });
    this->_function->instructions.push_back(instr);

    instr = {
        .opcode = ir::LLVMOpcode::RET,
        .type = types::toLLVMType(*node.getExpr()->getEvaluatedType()),
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)this->_function->instructions.size() - 1,
                },
            },
    };
    this->_function->blocks.at(this->_function->blocks.size() - 1)
        .instrIDs.push_back({
            .id = (int)this->_function->instructions.size(),
        });
    this->_function->instructions.push_back(instr);
}

void IRGenVisitor::visit(ast::expressions::IdentifierExpression& node) {
    ir::LLVMInstruction instr = {};

    if (node.isLvalue()) {
        instr = ir::LLVMInstruction{
            .opcode = ir::LLVMOpcode::ADD,
            .type =
                {
                    .type = ir::LLVMBasicType::PTR,
                },
            .operands =
                {
                    {
                        .kind = ir::LLVMValueKind::INSTR,
                        .id = this->_resolveIdentifier(node.getIdentifier()),
                    },
                    {
                        .kind = ir::LLVMValueKind::CONST,
                        .id = 0,
                    },
                },
        };
    } else {
        instr = ir::LLVMInstruction{
            .opcode = ir::LLVMOpcode::LOAD,
            .type = types::toLLVMType(*node.getEvaluatedType()),
            .operands =
                {
                    {
                        .kind = ir::LLVMValueKind::INSTR,
                        .id = this->_resolveIdentifier(node.getIdentifier()),
                    },
                },
        };
    }

    this->_function->blocks.at(this->_function->blocks.size() - 1)
        .instrIDs.push_back({
            .id = (int)this->_function->instructions.size(),
        });
    this->_function->instructions.push_back(instr);
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

    ir::LLVMInstruction instr = ir::LLVMInstruction{
        .opcode = ir::LLVMOpcode::STORE,
        .type = *this->_type,
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = (int)this->_function->instructions.size() - 1,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = this->_resolveIdentifier(this->_initialising),
                },
            },
    };
    this->_function->blocks.at(this->_function->blocks.size() - 1)
        .instrIDs.push_back({
            .id = (int)this->_function->instructions.size(),
        });
    this->_function->instructions.push_back(instr);
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
            this->_function->consts.push_back({
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
                            .id = (int)this->_function->consts.size() - 1,
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
        if (this->_global == nullptr) {
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
        } else {
            // die
        }
    }
}

void IRGenVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    ir::LLVMInstruction* temp = this->_instruction;

    node.getRhs()->accept(*this);
    const int rhs = this->_function->instructions.size() - 1;

    node.getLhs()->accept(*this);
    const int lhs = this->_function->instructions.size() - 1;

    ir::LLVMInstruction instr;

    // ASSUMES LHS ALWAYS RETURNS A POINTER, RHS ALWAYS RETURNS A VALUE
    // TODO:
    // once typing things exist this can be fixed to respect the lhs type correctly,
    // currently always assumes lhs dereferenced is the same type as the rhs for loading
    switch (node.getOp()) {
        case ast::expressions::binary::AssignmentExpressionType::MUL_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = this->_function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            const int loaded = this->_function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::MUL,
                .type = this->_function->instructions.at(rhs).type,
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
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            break;
        }
        // TODO: fix with signed/unsigned
        case ast::expressions::binary::AssignmentExpressionType::DIV_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = this->_function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            const int loaded = this->_function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::UDIV,
                .type = this->_function->instructions.at(rhs).type,
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
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            break;
        }
            // TODO: same as above
        case ast::expressions::binary::AssignmentExpressionType::MOD_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = this->_function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            const int loaded = this->_function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::UREM,
                .type = this->_function->instructions.at(rhs).type,
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
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::ADD_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = this->_function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };

            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            const int loaded = this->_function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::ADD,
                .type = this->_function->instructions.at(rhs).type,
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
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::SUB_ASSIGN: {
            instr = {
                .opcode = ir::LLVMOpcode::LOAD,
                .type = this->_function->instructions.at(rhs).type,
                .operands =
                    {
                        {
                            .kind = ir::LLVMValueKind::INSTR,
                            .id = lhs,
                        },
                    },
            };
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            const int loaded = this->_function->instructions.size() - 1;
            instr = {
                .opcode = ir::LLVMOpcode::SUB,
                .type = this->_function->instructions.at(rhs).type,
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
            this->_function->blocks.at(this->_function->blocks.size() - 1)
                .instrIDs.push_back({
                    .id = (int)this->_function->instructions.size(),
                });
            this->_function->instructions.push_back(instr);
            break;
        }
        // do these after signed things exist
        case ast::expressions::binary::AssignmentExpressionType::LEFT_ASSIGN: {
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::RIGHT_ASSIGN: {
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::AND_ASSIGN: {
            // instr = {
            //     .opcode = ir::LLVMOpcode::LOAD,
            //     .type = this->_function->instructions.at(rhs).type,
            //     .operands =
            //         {
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = lhs,
            //             },
            //         },
            // };
            // this->_function->blocks.at(this->_function->blocks.size() - 1)
            //     .instrIDs.push_back({
            //         .id = (int)this->_function->instructions.size(),
            //     });
            // this->_function->instructions.push_back(instr);
            // int loaded = this->_function->instructions.size() - 1;
            // instr = {
            //     .opcode = ir::LLVMOpcode::AND,
            //     .type = this->_function->instructions.at(rhs).type,
            //     .operands =
            //         {
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = loaded,
            //             },
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = rhs,
            //             },
            //         },
            // };
            // this->_function->blocks.at(this->_function->blocks.size() - 1)
            //     .instrIDs.push_back({
            //         .id = (int)this->_function->instructions.size(),
            //     });
            // this->_function->instructions.push_back(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::XOR_ASSIGN: {
            // instr = {
            //     .opcode = ir::LLVMOpcode::LOAD,
            //     .type = this->_function->instructions.at(rhs).type,
            //     .operands =
            //         {
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = lhs,
            //             },
            //         },
            // };
            // this->_function->blocks.at(this->_function->blocks.size() - 1)
            //     .instrIDs.push_back({
            //         .id = (int)this->_function->instructions.size(),
            //     });
            // this->_function->instructions.push_back(instr);
            // int loaded = this->_function->instructions.size() - 1;
            // instr = {
            //     .opcode = ir::LLVMOpcode::XOR,
            //     .type = this->_function->instructions.at(rhs).type,
            //     .operands =
            //         {
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = loaded,
            //             },
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = rhs,
            //             },
            //         },
            // };
            // this->_function->blocks.at(this->_function->blocks.size() - 1)
            //     .instrIDs.push_back({
            //         .id = (int)this->_function->instructions.size(),
            //     });
            // this->_function->instructions.push_back(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::OR_ASSIGN: {
            // instr = {
            //     .opcode = ir::LLVMOpcode::LOAD,
            //     .type = this->_function->instructions.at(rhs).type,
            //     .operands =
            //         {
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = lhs,
            //             },
            //         },
            // };
            // this->_function->blocks.at(this->_function->blocks.size() - 1)
            //     .instrIDs.push_back({
            //         .id = (int)this->_function->instructions.size(),
            //     });
            // this->_function->instructions.push_back(instr);
            // int loaded = this->_function->instructions.size() - 1;
            // instr = {
            //     .opcode = ir::LLVMOpcode::OR,
            //     .type = this->_function->instructions.at(rhs).type,
            //     .operands =
            //         {
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = loaded,
            //             },
            //             {
            //                 .kind = ir::LLVMValueKind::INSTR,
            //                 .id = rhs,
            //             },
            //         },
            // };
            // this->_function->blocks.at(this->_function->blocks.size() - 1)
            //     .instrIDs.push_back({
            //         .id = (int)this->_function->instructions.size(),
            //     });
            // this->_function->instructions.push_back(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::ASSIGN:
        default: {
            break;
        }
    }

    const int out = this->_function->instructions.size() - 1;

    instr = {
        .opcode = ir::LLVMOpcode::STORE,
        .type = {.type = ir::LLVMBasicType::VOID, .intSize = 0},
        .operands =
            {
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = out,
                },
                {
                    .kind = ir::LLVMValueKind::INSTR,
                    .id = lhs,
                },
            },
    };
    this->_function->blocks.at(this->_function->blocks.size() - 1)
        .instrIDs.push_back({
            .id = (int)this->_function->instructions.size(),
        });
    this->_function->instructions.push_back(instr);

    this->_instruction = temp;
}

}  // namespace thogcc::visitors
