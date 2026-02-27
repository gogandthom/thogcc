#pragma once

#include <cstdint>
#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class LogicalExpressionType : std::uint8_t { AND, OR };

class LogicalExpression : public BinaryExpressionBase {
   public:
    LogicalExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                      LogicalExpressionType op);

   private:
    LogicalExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
