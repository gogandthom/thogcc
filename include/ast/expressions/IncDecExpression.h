#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class IncDecExpression : public ExpressionBase {
   public:
    IncDecExpression(std::unique_ptr<ExpressionBase> expr, bool isDecrement = false,
                     bool isPrefix = false);

   private:
    std::unique_ptr<ExpressionBase> _expr;
    bool _isDecrement, _isPrefix;
};

}  // namespace thogcc::ast::expressions
