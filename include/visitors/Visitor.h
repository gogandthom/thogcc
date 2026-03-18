#pragma once

#include <string>

#include "ast/StorageClassSpecifier.h"
#include "ast/TypeQualifier.h"
#include "ast/TypeSpecifier.h"
#include "ast/fwd.h"  // IWYU pragma: keep
#include "ast/nodes.h"

namespace thogcc::visitors {

/// Visitor base class
/// This is strict, all visit() must be implemented.
/// See DefaultVisitor for fallthrough implementation.
class Visitor {
   public:
    Visitor() = default;
    virtual ~Visitor() = default;

    Visitor(const Visitor&) = delete;
    Visitor& operator=(const Visitor&) = delete;
    Visitor(Visitor&&) = delete;
    Visitor& operator=(Visitor&&) = delete;

#define V(NS, NAME) virtual void visit(NS::NAME& node) = 0;
    AST_NODES_ALL(V)
#undef V

    virtual void visitVal(ast::ValueNode<ast::TypeSpecifier>& valNode) {}
    virtual void visitVal(ast::ValueNode<ast::StorageClassSpecifier>& valNode) {}
    virtual void visitVal(ast::ValueNode<ast::TypeQualifier>& valNode) {}
    virtual void visitVal(ast::ValueNode<std::string>& valNode) {}
};

};  // namespace thogcc::visitors
