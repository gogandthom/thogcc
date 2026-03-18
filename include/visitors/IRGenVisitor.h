#pragma once

#include "visitors/Visitor.h"

namespace thogcc::visitors {

class IRGenVisitor : public Visitor {
   public:
    IRGenVisitor() = default;

#define V(NS, NAME) void visit(NS::NAME& node) override;
    AST_NODES_ALL(V)
#undef V

   private:
};

}  // namespace thogcc::visitors
