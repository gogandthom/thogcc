#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class LogicalExpressionType : std::uint8_t {
    AND,
    OR,
};

class LogicalExpression : public VisitableNode<LogicalExpression, BinaryExpressionBase> {
   public:
    LogicalExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                      LogicalExpressionType op)
        : VisitableNode(std::move(lhs), std::move(rhs)),  // Must call direct parent's constructor
          _op(op) {}

    LogicalExpressionType getOp() const {
        return _op;
    }

   private:
    LogicalExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
