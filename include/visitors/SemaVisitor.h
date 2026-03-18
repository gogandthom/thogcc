#pragma once

#include "ast/Node.h"
#include "ast/TypeQualifier.h"
#include "ast/TypeSpecifier.h"
#include "ast/declarations/Declaration.h"
#include "types/SymbolTable.h"
#include "visitors/RecursiveVisitor.h"

namespace thogcc::visitors {

class SemaVisitor : public RecursiveVisitor {
   public:
    SemaVisitor(types::SymbolTable& table) : _table(table) {}

    void visitVal(ast::ValueNode<ast::TypeSpecifier>& valNode) override;
    void visitVal(ast::ValueNode<ast::StorageClassSpecifier>& valNode) override;
    void visitVal(ast::ValueNode<ast::TypeQualifier>& valNode) override;
    void visitVal(ast::ValueNode<std::string>& valNode) override;

    void visit(ast::statements::CompoundStatement& node) override;
    void visit(ast::declarations::Declaration& node) override;

   private:
    types::SymbolTable& _table;
};

};  // namespace thogcc::visitors
