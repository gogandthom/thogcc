#pragma once

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

class IncDecExpression : public ExpressionBase {
   public:
    IncDecExpression(std::unique_ptr<ExpressionBase> expr, bool decrement = false)
        : _expr(std::move(expr)), _decrement(decrement){};

   private:
    std::unique_ptr<ExpressionBase> _expr;
    bool _decrement;
};

}  // namespace thogcc::ast::expressions::prefix
