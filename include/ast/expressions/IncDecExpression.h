#pragma once

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class IncDecExpression : public ExpressionBase {
   public:
    IncDecExpression(std::unique_ptr<ExpressionBase> expr, bool isDecrement = false,
                     bool isPrefix = false)
        : _expr(std::move(expr)), _isDecrement(isDecrement), _isPrefix(isPrefix){};

   private:
    std::unique_ptr<ExpressionBase> _expr;
    bool _isDecrement, _isPrefix;
};

}  // namespace thogcc::ast::expressions
