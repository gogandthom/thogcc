#pragma once

#include <type_traits>

#include "Visitor.h"
#include "ast/all.h"
#include "ast/nodes.h"
#include "ast/utils.h"

namespace thogcc::visitors {

template <typename T>
concept HasBase = requires { typename T::BaseType; };

/// Visitor base class to upcast nodes to their base type
/// (eg. CastExpression -> ExpressionBase -> Node)
class DefaultVisitor : public Visitor {
   public:
    template <typename T>
    void fallback(T& node) {
        if constexpr (std::is_same_v<T, ast::Node>) {
            return;  // Hit top of hierarchy
        } else if constexpr (HasBase<T>) {
            // upcast to BaseType
            this->visit(static_cast<typename T::BaseType&>(node));
        } else {
            static_assert(ast::always_false<T>, "Don't know what to upcast to.");
        }
    }

    // Default implementation is to fallback()
#define V(NS, NAME)                               \
    virtual void visit(NS::NAME& node) override { \
        fallback(node);                           \
    }
    AST_NODES_ALL(V)
#undef V
};

}  // namespace thogcc::visitors
