#pragma once

#include <cstdint>
#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

enum class RelationalExpressionType : std::uint8_t { L, G, LE, GE };

class RelationalExpression : public ExpressionBase {
   public:
    RelationalExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                         RelationalExpressionType type);

   private:
    std::unique_ptr<ExpressionBase> _lhs;
    std::unique_ptr<ExpressionBase> _rhs;
    RelationalExpressionType _type;
};

}  // namespace thogcc::ast::expressions
