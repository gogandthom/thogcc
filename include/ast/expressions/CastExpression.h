#pragma once

#include "ast/expressions/ExpressionBase.h"
namespace thogcc::ast::expressions {

class CastExpression : public ExpressionBase {
   public:
    CastExpression(/* TODO */, std::unique_ptr<ExpressionBase> expr);

   private:
    std::unique_ptr<ExpressionBase> _expr;
};

}  // namespace thogcc::ast::expressions
