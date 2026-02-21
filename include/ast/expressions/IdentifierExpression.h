#pragma once

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class IdentifierExpression : public ExpressionBase {
   public:
    IdentifierExpression(std::string identifier);

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::expressions
