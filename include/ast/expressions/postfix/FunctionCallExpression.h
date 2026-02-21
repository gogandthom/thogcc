#pragma once

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/ExpressionUnion.h"

namespace thogcc::ast::expressions::postfix {

class FunctionCallExpression : public ExpressionBase {
   public:
    FunctionCallExpression(std::unique_ptr<ExpressionUnion> expr,
                           std::unique_ptr<NodeList<ExpressionUnion>> argsExpr);

   private:
    std::unique_ptr<ExpressionUnion> _expr;
    std::unique_ptr<NodeList<ExpressionUnion>> _argsExpr;
};

}  // namespace thogcc::ast::expressions::postfix
