#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/Node.h"
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

class AddMultExpression : public VisitableNode<AddMultExpression, BinaryExpressionBase> {
   public:
    AddMultExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                      AddMultExpressionType op)
        : VisitableNode(std::move(lhs), std::move(rhs)),  // Must call direct parent's constructor
          _op(op){};

   private:
    AddMultExpressionType _op;
};

}  // namespace thogcc::ast::expressions::binary
