#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class BitwiseExpressionType : std::uint8_t { AND, OR, XOR };

class BitwiseExpression : public BinaryExpressionBase {
   public:
    BitwiseExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                      BitwiseExpressionType op)
        : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

   private:
    BitwiseExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
