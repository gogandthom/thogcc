#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class BitwiseExpressionType : std::uint8_t {
    AND,
    OR,
    XOR,
};

class BitwiseExpression : public VisitableNode<BitwiseExpression, BinaryExpressionBase> {
   public:
    BitwiseExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                      BitwiseExpressionType op)
        : VisitableNode(std::move(lhs), std::move(rhs)),  // Must call direct parent's constructor
          _op(op) {};

    BitwiseExpressionType getOp() {
        return _op;
    }

   private:
    BitwiseExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
