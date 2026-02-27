#pragma once

#include <cstdint>
#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class RelationalExpressionType : std::uint8_t { L, G, LE, GE };

class RelationalExpression : public BinaryExpressionBase {
   public:
    RelationalExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                         RelationalExpressionType op);

   private:
    RelationalExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
