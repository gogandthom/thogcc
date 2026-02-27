#pragma once

#include <cstdint>
#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class AddMultExpressionType : std::uint8_t {
    ADD,
    SUB,
    MUL,
    DIV,
    REM,
};

class AddMultExpression : public BinaryExpressionBase {
   public:
    AddMultExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                      AddMultExpressionType op);

   private:
    AddMultExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
