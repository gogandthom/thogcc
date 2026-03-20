#pragma once

#include <string>

#include "ast/Node.h"
#include "ast/TypeQualifier.h"
#include "ast/TypeSpecifier.h"
#include "ast/all.h"
#include "types/SymbolTable.h"
#include "types/Type.h"
#include "visitors/RecursiveVisitor.h"

namespace thogcc::visitors {

class SemaVisitor : public RecursiveVisitor {
   public:
    SemaVisitor(types::SymbolTable& table) : _table(table) {}

    void visitVal(ast::ValueNode<ast::TypeSpecifier>& valNode) override;
    void visitVal(ast::ValueNode<ast::StorageClassSpecifier>& valNode) override;
    void visitVal(ast::ValueNode<ast::TypeQualifier>& valNode) override;
    void visitVal(ast::ValueNode<std::string>& valNode) override;

    void visit(ast::declarations::Declaration& node) override;
    void visit(ast::declarations::FunctionDefinition& node) override;
    void visit(ast::declarations::ParameterDeclaration& node) override;
    void visit(ast::declarators::FunctionDeclarator& node) override;
    void visit(ast::declarators::IdentifierDeclarator& node) override;
    void visit(ast::declarators::InitDeclarator& node) override;
    void visit(ast::declarators::PointerDeclarator& node) override;
    void visit(ast::expressions::IdentifierExpression& node) override;
    void visit(ast::expressions::ListExpression& node) override;
    void visit(ast::expressions::PrimaryExpression& node) override;
    void visit(ast::expressions::binary::AddMultExpression& node) override;
    void visit(ast::expressions::binary::AssignmentExpression& node) override;
    void visit(ast::expressions::binary::EqualityExpression& node) override;
    void visit(ast::expressions::postfix::FunctionCallExpression& node) override;
    void visit(ast::statements::CompoundStatement& node) override;

   private:
    static std::shared_ptr<types::Type> getPromotedType(const std::shared_ptr<types::Type>& lhs,
                                                        const std::shared_ptr<types::Type>& rhs);

    types::SymbolTable _table;
    types::Type _curType;
};

};  // namespace thogcc::visitors
