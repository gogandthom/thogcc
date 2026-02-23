#pragma once

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class MemberAccessExpression : public ExpressionBase {
   public:
    MemberAccessExpression(std::unique_ptr<ExpressionBase> expr, bool exprIsPtr = false);

   private:
    std::unique_ptr<ExpressionBase> _expr;
    bool _exprIsPtr;
};

}  // namespace thogcc::ast::expressions::postfix
