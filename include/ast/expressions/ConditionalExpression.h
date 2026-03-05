#pragma once

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ConditionalExpression : public ExpressionBase {
   public:
    ConditionalExpression(std::unique_ptr<ExpressionBase> cond,
                          std::unique_ptr<ExpressionBase> ifExpr,
                          std::unique_ptr<ExpressionBase> elseExpr)
        : _cond(std::move(cond)), _ifExpr(std::move(ifExpr)), _elseExpr(std::move(elseExpr)){};

   private:
    std::unique_ptr<ExpressionBase> _cond, _ifExpr, _elseExpr;
};

}  // namespace thogcc::ast::expressions
