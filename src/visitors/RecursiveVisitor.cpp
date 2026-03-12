#include "visitors/RecursiveVisitor.h"

#include <cassert>
#include <cstddef>
#include <variant>

#include "ast/all.h"

namespace thogcc::visitors {

void RecursiveVisitor::visit(ast::Node& /* node */) {
    assert(false);
}

void RecursiveVisitor::visit(ast::NodeListBase& node) {
    for (size_t i = 0; i < node.size(); ++i) {
        node.getRawNode(i).accept(*this);
    }
}

void RecursiveVisitor::visit(ast::ValueNodeBase& /* node */) {}

void RecursiveVisitor::visit(ast::DeclarationSpecifiers& node) {
    traverse(node.getStorageClassSpecifier());
    traverse(node.getTypeSpecifier());
}

void RecursiveVisitor::visit(ast::EnumSpecifier& node) {
    traverse(node.getDeclarators());
}

void RecursiveVisitor::visit(ast::StructSpecifier& node) {
    traverse(node.getDeclarations());
}

void RecursiveVisitor::visit(ast::TypeName& node) {
    traverse(node.getSpecifiers());
    traverse(node.getDeclarator());
}

void RecursiveVisitor::visit(ast::declarations::DeclarationBase& /* node */) {}

void RecursiveVisitor::visit(ast::declarations::Declaration& node) {
    traverse(node.getSpecifiers());
    traverse(node.getDeclarators());
}

void RecursiveVisitor::visit(ast::declarations::FunctionDefinition& node) {
    traverse(node.getSpecifiers());
    traverse(node.getDeclarator());
    traverse(node.getDeclarations());
    traverse(node.getStatement());
}

void RecursiveVisitor::visit(ast::declarations::ParameterDeclaration& node) {
    traverse(node.getSpecifiers());
    traverse(node.getDecl());
}

void RecursiveVisitor::visit(ast::declarations::StructDeclaration& node) {
    traverse(node.getQualifiers());
    traverse(node.getDeclarators());
}

void RecursiveVisitor::visit(ast::declarators::DeclaratorBase& /* node */) {}

void RecursiveVisitor::visit(ast::declarators::ArrayDeclarator& node) {
    traverse(node.getBase());
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::declarators::EnumValueDeclarator& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    traverse(node.getBase());
    traverse(node.getIdentifiers());
    traverse(node.getParams());
}

void RecursiveVisitor::visit(ast::declarators::IdentifierDeclarator& /* node */) {}

void RecursiveVisitor::visit(ast::declarators::InitDeclarator& node) {
    traverse(node.getDecl());
    traverse(node.getInitializer());
}

void RecursiveVisitor::visit(ast::declarators::PointerDeclarator& node) {
    traverse(node.getPtr());
}

void RecursiveVisitor::visit(ast::declarators::StructMemberDeclarator& node) {
    traverse(node.getDecl());
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::expressions::ExpressionBase& /* node */) {}

void RecursiveVisitor::visit(ast::expressions::CastExpression& node) {
    traverse(node.getExpr());
    traverse(node.getTypeName());
}

void RecursiveVisitor::visit(ast::expressions::ConditionalExpression& node) {
    traverse(node.getCond());
    traverse(node.getIfExpr());
    traverse(node.getElseExpr());
}

void RecursiveVisitor::visit(ast::expressions::ConstantExpression& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::expressions::IdentifierExpression& /* node */) {}

void RecursiveVisitor::visit(ast::expressions::IncDecExpression& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::expressions::Initializer& node) {
    std::visit([this](auto&& child) { traverse(child.get()); }, node.getChild());
}

void RecursiveVisitor::visit(ast::expressions::ListExpression& node) {
    traverse(node.getList());
}

void RecursiveVisitor::visit(ast::expressions::PrimaryExpression& /* node */) {}

void RecursiveVisitor::visit(ast::expressions::binary::BinaryExpressionBase& node) {
    traverse(node.getLhs());
    traverse(node.getRhs());
}

void RecursiveVisitor::visit(ast::expressions::binary::AddMultExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::binary::BitwiseExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::binary::EqualityExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::binary::LogicalExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::binary::RelationalExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::binary::ShiftExpression& node) {
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void RecursiveVisitor::visit(ast::expressions::postfix::ArrayAccessExpression& node) {
    traverse(node.getArrayExpr());
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::expressions::postfix::FunctionCallExpression& node) {
    traverse(node.getExpr());
    traverse(node.getArgs());
}

void RecursiveVisitor::visit(ast::expressions::postfix::MemberAccessExpression& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::expressions::prefix::SizeofExpression& node) {
    std::visit([this](auto&& expr) { traverse(expr.get()); }, node.getExpr());
}

void RecursiveVisitor::visit(ast::expressions::prefix::UnaryOperatorExpression& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::statements::StatementBase& /* node */) {}

void RecursiveVisitor::visit(ast::statements::CompoundStatement& node) {
    traverse(node.getStatementList());
    traverse(node.getDeclarationList());
}

void RecursiveVisitor::visit(ast::statements::ExpressionStatement& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::statements::GotoStatement& /* node */) {}

void RecursiveVisitor::visit(ast::statements::IfStatement& node) {
    traverse(node.getExpr());
    traverse(node.getIfStatement());
    traverse(node.getElseStatement());
}

void RecursiveVisitor::visit(ast::statements::IterationStatement& node) {
    traverse(node.getStatement());
    traverse(node.getInitExpr());
    traverse(node.getCondExpr());
    traverse(node.getUpdateExpr());
}

void RecursiveVisitor::visit(ast::statements::LabelledStatement& node) {
    traverse(node.getStatement());
}

void RecursiveVisitor::visit(ast::statements::LoopControlStatement& /* node */) {}

void RecursiveVisitor::visit(ast::statements::ReturnStatement& node) {
    traverse(node.getExpr());
}

void RecursiveVisitor::visit(ast::statements::SwitchCaseStatement& node) {
    traverse(node.getExpr());
    traverse(node.getStatement());
}

void RecursiveVisitor::visit(ast::statements::SwitchStatement& node) {
    traverse(node.getExpr());
    traverse(node.getStatement());
}

}  // namespace thogcc::visitors
