#pragma once

#include <cstdint>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

enum class UnaryOperatorType : std::uint8_t {
    ADDRESSOF,
    INDIRECTION,
    PLUS,
    MINUS,
    BITWISE_NOT,
    LOGICAL_NOT,
    SIZEOF,
};

class UnaryOperatorExpression : public ExpressionBase {
   public:
    UnaryOperatorExpression(UnaryOperatorType type, std::unique_ptr<ExpressionBase> expr);
    UnaryOperatorExpression(UnaryOperatorType type /* TODO , type_name */);

   private:
    UnaryOperatorType _type;
    std::unique_ptr<ExpressionBase> _expr;
};

}  // namespace thogcc::ast::expressions::prefix
