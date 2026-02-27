#include "ast/expressions/binary/BitwiseExpression.h"

namespace thogcc::ast::expressions::binary {

BitwiseExpression::BitwiseExpression(std::unique_ptr<ExpressionBase> lhs,
                                     std::unique_ptr<ExpressionBase> rhs, BitwiseExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
