#pragma once

#include <cstdint>
#include <string_view>

#include "ast/fwd.h"  // IWYU pragma: keep
#include "ast/nodes.h"

namespace thogcc::ast {

/// Always returns false.
/// Useful for making static_assert work nicely across different C++ versions
template <class...>
constexpr bool always_false = false;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define V(NS, NAME) NAME,
/// Enum class for all types of node
enum class NodeKind : std::uint8_t { AST_NODES_ALL(V) };
#undef V

template <typename T>
struct NodeKindTrait;

#define V(NS, NAME)                                      \
    template <>                                          \
    struct NodeKindTrait<NS::NAME> {                     \
        static constexpr NodeKind kind = NodeKind::NAME; \
    };
AST_NODES_ALL(V)
#undef V

/// Helper to provide string representation of any NodeKind.
constexpr std::string_view nodeKindName(NodeKind kind) {
    switch (kind) {
#define V(NS, NAME)      \
    case NodeKind::NAME: \
        return #NAME;
        AST_NODES_ALL(V)
#undef V
    }
};

}  // namespace thogcc::ast
