#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class CastExpression : public ExpressionBase {
   public:
    CastExpression(std::unique_ptr<Node> typeName /* TODO better type than Node? */,
                   std::unique_ptr<ExpressionBase> expr);

   private:
    std::unique_ptr<ExpressionBase> _expr;
};

}  // namespace thogcc::ast::expressions
