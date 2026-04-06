#pragma once

#include <memory>
#include <utility>
#include <variant>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class Initializer : public VisitableNode<Initializer, ExpressionBase> {
   public:
    Initializer(std::unique_ptr<ExpressionBase> expr) : _child(std::move(expr)) {}
    Initializer(std::unique_ptr<NodeList<Initializer>> list) : _child(std::move(list)) {}

    auto& getChild() const {
        return _child;
    }

   private:
    std::variant<std::unique_ptr<ExpressionBase>, std::unique_ptr<NodeList<Initializer>>> _child;
};

}  // namespace thogcc::ast::expressions
