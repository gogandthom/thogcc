#pragma once

#include "ast/utils.h"

namespace thogcc::visitors {

class Visitor {
   public:
    virtual ~Visitor() = default;

#define V(NS, NAME) virtual void visit(NS::NAME& node) = 0;
    AST_NODES_ALL(V)
#undef V
};

};  // namespace thogcc::visitors
