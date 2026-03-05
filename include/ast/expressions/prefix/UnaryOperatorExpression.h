#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

enum class UnaryOperatorType : std::uint8_t {
    ADDRESSOF,
    INDIRECTION,
    PLUS,
    MINUS,
    BITWISE_NOT,
    LOGICAL_NOT,
};

class UnaryOperatorExpression : public ExpressionBase {
   public:
    UnaryOperatorExpression(UnaryOperatorType op, std::unique_ptr<ExpressionBase> expr)
        : _op(op), _expr(std::move(expr)){};

   private:
    UnaryOperatorType _op;
    std::unique_ptr<ExpressionBase> _expr;
};

}  // namespace thogcc::ast::expressions::prefix
