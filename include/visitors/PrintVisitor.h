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
    void visit(ast::declarations::FunctionDefinition& node) override;
    void visit(ast::declarators::FunctionDeclarator& node) override;
    void visit(ast::declarators::IdentifierDeclarator& node) override;
    void visit(ast::expressions::ListExpression& node) override;
    void visit(ast::expressions::PrimaryExpression& node) override;
    void visit(ast::statements::CompoundStatement& node) override;
    void visit(ast::statements::ReturnStatement& node) override;

   private:
    void printNode(int id, ast::Node& node);

    template <typename T>
    void visitChild(int cur, std::string_view label, T* child);

    std::ostream& _out;
    int _id = 0;
};

}  // namespace thogcc::visitors
