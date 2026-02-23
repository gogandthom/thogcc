#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class IncDecExpression : public ExpressionBase {
   public:
    IncDecExpression(std::unique_ptr<ExpressionBase> expr, bool decrement = false);

   private:
    std::unique_ptr<ExpressionBase> _expr;
    bool _decrement;
};

}  // namespace thogcc::ast::expressions::postfix
