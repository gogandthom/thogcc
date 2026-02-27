#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ConditionalExpression : public ExpressionBase {
   public:
    ConditionalExpression(std::unique_ptr<ExpressionBase> cond,
                          std::unique_ptr<ExpressionBase> ifExpr,
                          std::unique_ptr<ExpressionBase> elseExpr);

   private:
    std::unique_ptr<ExpressionBase> _cond, _ifExpr, _elseExpr;
};

}  // namespace thogcc::ast::expressions
