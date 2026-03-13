#pragma once

#include <ostream>
#include <string_view>

#include "ast/Node.h"
#include "ast/fwd.h"
#include "visitors/Visitor.h"

namespace thogcc::visitors {

/// Mermaid-elk graph printer
class PrintVisitor : public Visitor {
   public:
    PrintVisitor(std::ostream& out);

#define V(NS, NAME) virtual void visit(NS::NAME& node) override;
    AST_NODES_ALL(V)
#undef V

   private:
    void printNode(int id, ast::Node& node);

    template <typename T>
    void visitChild(int cur, std::string_view label, T* child);

    std::ostream& _out;
    int _id = 0;
};

}  // namespace thogcc::visitors
