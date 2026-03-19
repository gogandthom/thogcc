#pragma once

#include "ast/Node.h"
#include "ir/llvm.h"
#include "visitors/DefaultVisitor.h"

namespace thogcc::visitors {

class IRGenVisitor : public DefaultVisitor {
   public:
    IRGenVisitor(std::string srcFilePath);

    void visit(ast::Node& node) override;
    void visit(ast::NodeListBase& node) override;

    void visit(ast::declarations::Declaration& node) override;
    void visit(ast::declarations::FunctionDefinition& node) override;

    void visit(ast::declarations::ParameterDeclaration& node) override;

    void visit(ast::declarators::FunctionDeclarator& node) override;
    void visit(ast::declarators::InitDeclarator& node) override;
    void visit(ast::declarators::IdentifierDeclarator& node) override;

    void visit(ast::statements::CompoundStatement& node) override;
    void visit(ast::statements::ExpressionStatement& node) override;
    void visit(ast::statements::ReturnStatement& node) override;

    void visit(ast::expressions::binary::AssignmentExpression& node) override;
    void visit(ast::expressions::ListExpression& node) override;

    ir::LLVMModule getModule();

    // #define V(NS, NAME) void visit(NS::NAME& node) override;
    //     AST_NODES_ALL(V)
    // #undef V

   private:
    ir::LLVMModule _module;
    ir::LLVMFunction* _function = nullptr;
    ir::LLVMGlobal* _global = nullptr;
    ir::LLVMInstruction* _instruction = nullptr;
    ir::LLVMType* type = nullptr;
};

}  // namespace thogcc::visitors
