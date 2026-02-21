#pragma once

#include <variant>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class PrimaryExpression : public ExpressionBase {
   public:
    PrimaryExpression(int value);
    PrimaryExpression(float value);
    PrimaryExpression(std::string value);

   private:
    // TODO introduce other types (uint, long, double, etc)?
    std::variant<int, float, std::string> _value;
};

}  // namespace thogcc::ast::expressions
