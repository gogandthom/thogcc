#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

enum class AssignmentExpressionType : std::uint8_t {
    ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    LEFT_ASSIGN,
    RIGHT_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    OR_ASSIGN,
};

class AssignmentExpression : public VisitableNode<AssignmentExpression, BinaryExpressionBase> {
   public:
    AssignmentExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                         AssignmentExpressionType op)
        : VisitableNode(std::move(lhs),
                        std::move(rhs)),  // Must call direct parent's constructor
          _op(op) {}

    AssignmentExpressionType getOp() const {
        return _op;
    }

   private:
    AssignmentExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
