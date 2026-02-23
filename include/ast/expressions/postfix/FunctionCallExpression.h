#pragma once

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class FunctionCallExpression : public ExpressionBase {
   public:
    FunctionCallExpression(std::unique_ptr<ExpressionBase> expr,
                           std::unique_ptr<NodeList<ExpressionBase>> argsExpr);

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::unique_ptr<NodeList<ExpressionBase>> _argsExpr;
};

}  // namespace thogcc::ast::expressions::postfix
