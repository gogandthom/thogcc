#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "ast/Node.h"
#include "ast/fwd.h"
#include "ir/LLVMModule.h"
#include "ir/LLVMType.h"
#include "visitors/DefaultVisitor.h"

namespace thogcc::visitors {

class IRGenVisitor : public DefaultVisitor {
   public:
    IRGenVisitor(ir::LLVMModule& module);

    void visit(ast::Node& node) override;
    void visit(ast::NodeListBase& node) override;

    void visit(ast::declarations::Declaration& node) override;
    void visit(ast::declarations::FunctionDefinition& node) override;
    void visit(ast::declarations::ParameterDeclaration& node) override;
    void visit(ast::declarators::FunctionDeclarator& node) override;
    void visit(ast::declarators::IdentifierDeclarator& node) override;
    void visit(ast::declarators::InitDeclarator& node) override;
    void visit(ast::declarators::PointerDeclarator& node) override;
    void visit(ast::expressions::IdentifierExpression& node) override;
    void visit(ast::expressions::Initializer& node) override;
    void visit(ast::expressions::ListExpression& node) override;
    void visit(ast::expressions::PrimaryExpression& node) override;
    void visit(ast::expressions::binary::AddMultExpression& node) override;
    void visit(ast::expressions::binary::AssignmentExpression& node) override;
    void visit(ast::expressions::binary::BitwiseExpression& node) override;
    void visit(ast::expressions::binary::EqualityExpression& node) override;
    void visit(ast::statements::CompoundStatement& node) override;
    void visit(ast::statements::ExpressionStatement& node) override;
    void visit(ast::statements::IfStatement& node) override;
    void visit(ast::statements::ReturnStatement& node) override;

    // #define V(NS, NAME) void visit(NS::NAME& node) override;
    //     AST_NODES_ALL(V)
    // #undef V

   private:
    ir::LLVMModule& _module;
    ir::LLVMFunction* _function = nullptr;
    ir::LLVMGlobal* _global = nullptr;
    ir::LLVMInstruction* _instruction = nullptr;
    ir::LLVMType* _type = nullptr;
    std::string_view _initialising;

    typedef std::map<std::string_view, ir::LLVMValueID> IdentifierResolutionLayer;
    std::vector<IdentifierResolutionLayer> _identifiers;

    ir::LLVMValueID _resolveIdentifier(std::string_view name);
    std::map<std::string_view, ir::LLVMValueID>& _currentIdentifiers();

    std::size_t _emitInstr(const ir::LLVMInstruction& instr);
    std::size_t _evaluateAsRValue(ast::expressions::ExpressionBase& node);
    ir::LLVMBasicBlock& _createBlock(std::string label);
};

}  // namespace thogcc::visitors
