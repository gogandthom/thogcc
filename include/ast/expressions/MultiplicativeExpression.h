#pragma once

#include <cstdint>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

enum class MultiplicativeExpressionType : std::uint8_t {
    MUL,
    DIV,
    REM,
};

class MultiplicativeExpression : public ExpressionBase {
   public:
    MultiplicativeExpression(std::unique_ptr<ExpressionBase> lhs,
                             std::unique_ptr<ExpressionBase> rhs,
                             MultiplicativeExpressionType type);

   private:
    std::unique_ptr<ExpressionBase> _lhs;
    std::unique_ptr<ExpressionBase> _rhs;
    MultiplicativeExpressionType _type;
};

}  // namespace thogcc::ast::expressions
