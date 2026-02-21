#pragma once

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/ExpressionUnion.h"

namespace thogcc::ast::expressions::postfix {

class ArrayAccessExpression : public ExpressionBase {
   public:
    ArrayAccessExpression(std::unique_ptr<ExpressionUnion> expr,
                          std::unique_ptr<ExpressionUnion> arrayExpr);

   private:
    std::unique_ptr<ExpressionUnion> _expr, _arrayExpr;
};

}  // namespace thogcc::ast::expressions::postfix
