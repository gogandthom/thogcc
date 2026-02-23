#pragma once

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ShiftExpression : public ExpressionBase {
   public:
    ShiftExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                    bool isRightShift = false);

   private:
    std::unique_ptr<ExpressionBase> _lhs;
    std::unique_ptr<ExpressionBase> _rhs;
    bool _isRightShift;
};

}  // namespace thogcc::ast::expressions
