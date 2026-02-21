#pragma once

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/ExpressionUnion.h"

namespace thogcc::ast::expressions::prefix {

class IncDecExpression : public ExpressionBase {
   public:
    IncDecExpression(std::unique_ptr<ExpressionUnion> expr, bool decrement = false);

   private:
    std::unique_ptr<ExpressionUnion> _expr;
    bool _decrement;
};

}  // namespace thogcc::ast::expressions::prefix
