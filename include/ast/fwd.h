#pragma once

#include "ast/nodes.h"

// Forward declarations
#define V(NS, NAME)        \
    namespace thogcc::NS { \
    class NAME;            \
    }
AST_NODES_ALL(V)
#undef V

// and for templates
namespace thogcc::ast {
template <typename T>
class NodeList;
template <typename E>
class ValueNode;
}  // namespace thogcc::ast
