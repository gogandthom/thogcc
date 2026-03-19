#include "visitors/IRGenVisitor.h"

#include <iostream>

#include "errors/errors.h"

namespace thogcc::visitors {

IRGenVisitor::IRGenVisitor(std::string srcFilePath) {
    this->_module = {};
    this->_module.srcFileName = srcFilePath;
    this->_function = nullptr;
}

ir::LLVMModule IRGenVisitor::getModule() {
    return this->_module;
}

void IRGenVisitor::visit(ast::Node& node) {
    std::cout << "yeep tain " << ast::nodeKindName(node.getKind()) << std::endl;
}

void IRGenVisitor::visit(ast::NodeListBase& node) {
    for (size_t i = 0; i < node.size(); i++) {
        node.getRawNode(i).accept(*this);
    }
}

void IRGenVisitor::visit(ast::declarations::Declaration& node) {
    if (this->_function == nullptr) {
        // global declaration
        std::cout << "global" << std::endl;
        ir::LLVMGlobal global = {};
        this->_global = &global;
        node.getSpecifiers()->accept(*this);
        node.getDeclarators()->accept(*this);
        this->_global = nullptr;
        this->_module.globals.push_back(global);
    } else {
        // local declaration
        std::cout << "local" << std::endl;
        ir::LLVMType type = {};
        this->type = &type;
        node.getSpecifiers()->accept(*this);
        ir::LLVMInstruction instr = {};
        this->_instruction = &instr;
        node.getDeclarators()->accept(*this);
        this->_instruction = nullptr;
        this->type = nullptr;
        this->_function->instructions.push_back(instr);
    }
}

void IRGenVisitor::visit(ast::declarations::FunctionDefinition& node) {
    std::cout << "global function" << std::endl;
    ir::LLVMFunction func = {};
    this->_function = &func;
    node.getSpecifiers()->accept(*this);
    node.getDeclarator()->accept(*this);
    if (node.getDeclarations()) node.getDeclarations()->accept(*this);
    if (node.getStatement()) node.getStatement()->accept(*this);
    this->_function = nullptr;
    this->_module.functions.push_back(func);
}

void IRGenVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    this->_function->name = node.getIdentifier();
    // TODO
}

void IRGenVisitor::visit(ast::declarators::InitDeclarator& node) {
    node.getDecl()->accept(*this);
    if (node.getInitializer()) node.getInitializer()->accept(*this);
}

void IRGenVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    if (this->_global) {
        // identifier for global so yay
        this->_global->name = node.getIdentifier();
    }
    // TODO
}

void IRGenVisitor::visit(ast::statements::CompoundStatement& node) {
    node.getDeclarationList()->accept(*this);
    node.getStatementList()->accept(*this);
}

void IRGenVisitor::visit(ast::statements::ExpressionStatement& node) {
    node.getExpr()->accept(*this);
}

void IRGenVisitor::visit(ast::expressions::ListExpression& node) {
    for (size_t i = 0; i < node.getList()->size(); i++) {
        node.getList()->getRawNode(i).accept(*this);
    }
}

void IRGenVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    ir::LLVMInstruction* temp = this->_instruction;

    node.getRhs()->accept(*this);
    int rhs = this->_function->instructions.size() - 1;

    node.getLhs()->accept(*this);
    int lhs = this->_function->instructions.size() - 1;

    ir::LLVMInstruction instr = {};

    // ASSUMES LHS ALWAYS RETURNS A POINTER, RHS ALWAYS RETURNS A VALUE
    // TODO:
    // once typing things exist this can be fixed to respect the lhs type correctly,
    // currently always assumes lhs dereferenced is the same type as the rhs for loading
    switch (node.getOp()) {
        case ast::expressions::binary::AssignmentExpressionType::MUL_ASSIGN:
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
            this->_function->instructions.push_back(instr);
            int loaded = this->_function->instructions.size() - 1;
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
            this->_function->instructions.push_back(instr);
            break;

            // TODO: fix with signed/unsigned
        case ast::expressions::binary::AssignmentExpressionType::DIV_ASSIGN:
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
            this->_function->instructions.push_back(instr);
            int loaded = this->_function->instructions.size() - 1;
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
            this->_function->instructions.push_back(instr);
            break;

            // TODO: same as above
        case ast::expressions::binary::AssignmentExpressionType::MOD_ASSIGN:
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
            this->_function->instructions.push_back(instr);
            int loaded = this->_function->instructions.size() - 1;
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
            this->_function->instructions.push_back(instr);
            break;

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
            this->_function->instructions.push_back(instr);
            int loaded = this->_function->instructions.size() - 1;
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
            this->_function->instructions.push_back(instr);
            break;
        }
        case ast::expressions::binary::AssignmentExpressionType::SUB_ASSIGN:
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
            this->_function->instructions.push_back(instr);
            int loaded = this->_function->instructions.size() - 1;
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
            this->_function->instructions.push_back(instr);
            break;

            // do these after signed things exist
        case ast::expressions::binary::AssignmentExpressionType::LEFT_ASSIGN:
            break;
        case ast::expressions::binary::AssignmentExpressionType::RIGHT_ASSIGN:
            break;

        case ast::expressions::binary::AssignmentExpressionType::AND_ASSIGN:
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
            // this->_function->instructions.push_back(instr);
            break;
        case ast::expressions::binary::AssignmentExpressionType::XOR_ASSIGN:
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
            // this->_function->instructions.push_back(instr);
            break;
        case ast::expressions::binary::AssignmentExpressionType::OR_ASSIGN:
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
            // this->_function->instructions.push_back(instr);
            break;

        case ast::expressions::binary::AssignmentExpressionType::ASSIGN:
        default:
            break;
    }

    int out = this->_function->instructions.size() - 1;

    instr = {
        .opcode = ir::LLVMOpcode::STORE,
        .type =
            {
                .type = ir::LLVMBasicType::VOID,
            },
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
    this->_function->instructions.push_back(instr);
}

}  // namespace thogcc::visitors
