#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class ShiftExpression : public BinaryExpressionBase {
   public:
    ShiftExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                    bool isRightShift = false);

   private:
    bool _isRightShift;
};

}  // namespace thogcc::ast::expressions::binary
