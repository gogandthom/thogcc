#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class IncDecExpression : public VisitableNode<IncDecExpression, ExpressionBase> {
   public:
    IncDecExpression(std::unique_ptr<ExpressionBase> expr, bool isDecrement = false,
                     bool isPrefix = false)
        : _expr(std::move(expr)), _isDecrement(isDecrement), _isPrefix(isPrefix) {}

    auto* getExpr() const {
        return _expr.get();
    }

    bool getIsDecrement() const {
        return _isDecrement;
    }

    bool getIsPrefix() const {
        return _isPrefix;
    }

   private:
    std::unique_ptr<ExpressionBase> _expr;
    bool _isDecrement, _isPrefix;
};

}  // namespace thogcc::ast::expressions
