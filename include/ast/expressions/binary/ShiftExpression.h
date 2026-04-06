#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class ShiftExpression : public VisitableNode<ShiftExpression, BinaryExpressionBase> {
   public:
    ShiftExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                    bool isRightShift = false)
        : VisitableNode(std::move(lhs), std::move(rhs)),  // Must call direct parent's constructor
          _isRightShift(isRightShift) {}

    bool getIsRightShift() const {
        return _isRightShift;
    }

   private:
    bool _isRightShift;
};

}  // namespace thogcc::ast::expressions::binary
