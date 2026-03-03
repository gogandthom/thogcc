#pragma once

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class ShiftExpression : public BinaryExpressionBase {
   public:
    ShiftExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                    bool isRightShift = false)
        : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _isRightShift(isRightShift){};

   private:
    bool _isRightShift;
};

}  // namespace thogcc::ast::expressions::binary
