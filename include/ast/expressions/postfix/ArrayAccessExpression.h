#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class ArrayAccessExpression : public VisitableNode<ArrayAccessExpression, ExpressionBase> {
   public:
    ArrayAccessExpression(std::unique_ptr<ExpressionBase> expr,
                          std::unique_ptr<ExpressionBase> arrayExpr)
        : _expr(std::move(expr)), _arrayExpr(std::move(arrayExpr)){};

   private:
    std::unique_ptr<ExpressionBase> _expr, _arrayExpr;
};

}  // namespace thogcc::ast::expressions::postfix
