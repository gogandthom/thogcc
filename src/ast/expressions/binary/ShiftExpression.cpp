#include "ast/expressions/binary/ShiftExpression.h"

namespace thogcc::ast::expressions::binary {

ShiftExpression::ShiftExpression(std::unique_ptr<ExpressionBase> lhs,
                                 std::unique_ptr<ExpressionBase> rhs, bool isRightShift)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _isRightShift(isRightShift){};

}
