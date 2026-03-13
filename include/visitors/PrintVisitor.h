#pragma once

#include <ostream>
#include <string_view>

#include "ast/Node.h"
#include "ast/fwd.h"
#include "visitors/DefaultVisitor.h"

namespace thogcc::visitors {

/// Mermaid-elk graph printer
class PrintVisitor : public DefaultVisitor {
   public:
    PrintVisitor(std::ostream& out);

    void visit(ast::Node& node) override;
    void visit(ast::NodeListBase& node) override;
    void visit(ast::ValueNodeBase& node) override;

    void visit(ast::DeclarationSpecifiers& node) override;
    void visit(ast::declarations::Declaration& node) override;
    void visit(ast::declarations::FunctionDefinition& node) override;

    // declarators
    void visit(ast::declarators::ArrayDeclarator& node) override;
    void visit(ast::declarators::FunctionDeclarator& node) override;
    void visit(ast::declarators::IdentifierDeclarator& node) override;
    void visit(ast::declarators::InitDeclarator& node) override;
    void visit(ast::declarators::EnumValueDeclarator& node) override;

    // expressions
    void visit(ast::expressions::CastExpression& node) override;
    void visit(ast::expressions::ConditionalExpression& node) override;
    void visit(ast::expressions::ConstantExpression& node) override;
    void visit(ast::expressions::IdentifierExpression& node) override;
    void visit(ast::expressions::IncDecExpression& node) override;
    void visit(ast::expressions::Initializer& node) override;
    void visit(ast::expressions::ListExpression& node) override;
    void visit(ast::expressions::PrimaryExpression& node) override;

    void visit(ast::expressions::postfix::ArrayAccessExpression& node) override;
    void visit(ast::expressions::postfix::FunctionCallExpression& node) override;
    void visit(ast::expressions::postfix::MemberAccessExpression& node) override;

    void visit(ast::expressions::prefix::SizeofExpression& node) override;
    void visit(ast::expressions::prefix::UnaryOperatorExpression& node) override;

    // statements
    void visit(ast::statements::CompoundStatement& node) override;
    void visit(ast::statements::ReturnStatement& node) override;

    // specifiers
    void visit(ast::EnumSpecifier& node) override;

   private:
    void printNode(int id, ast::Node& node);

    template <typename T>
    void visitChild(int cur, std::string_view label, T* child);

    std::ostream& _out;
    int _id = 0;
};

}  // namespace thogcc::visitors
