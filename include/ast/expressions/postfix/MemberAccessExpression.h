#pragma once

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/ExpressionUnion.h"

namespace thogcc::ast::expressions::postfix {

class MemberAccessExpression : public ExpressionBase {
   public:
    MemberAccessExpression(std::unique_ptr<ExpressionUnion> expr, bool exprIsPtr = false);

   private:
    std::unique_ptr<ExpressionUnion> _expr;
    bool _exprIsPtr;
};

}  // namespace thogcc::ast::expressions::postfix
