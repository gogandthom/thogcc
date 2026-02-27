#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ConstantExpression : public ExpressionBase {
   public:
    ConstantExpression(std::unique_ptr<ExpressionBase> expr);

   private:
    std::unique_ptr<ExpressionBase> _expr;
};

}  // namespace thogcc::ast::expressions
