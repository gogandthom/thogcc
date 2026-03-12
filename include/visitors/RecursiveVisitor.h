#pragma once

#include "ast/fwd.h"  // IWYU pragma: keep
#include "ast/nodes.h"
#include "visitors/Visitor.h"

namespace thogcc::visitors {

/// Visitor base to blindly recurse into all children
/// Maybe useful for different passes to only override what we care about
class RecursiveVisitor : public Visitor {
   public:
    RecursiveVisitor() = default;

    template <typename T>
    void traverse(T* child) {
        if (child) child->accept(*this);
    }

#define V(NS, NAME) virtual void visit(NS::NAME& node) override;
    AST_NODES_ALL(V)
#undef V
};

}  // namespace thogcc::visitors
