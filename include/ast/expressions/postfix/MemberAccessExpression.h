#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class MemberAccessExpression : public ExpressionBase {
   public:
    MemberAccessExpression(std::unique_ptr<ExpressionBase> expr, std::string identifier,
                           bool exprIsPtr = false);

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::string _identifier;
    bool _exprIsPtr;
};

}  // namespace thogcc::ast::expressions::postfix
