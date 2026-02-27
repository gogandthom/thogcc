#include "ast/expressions/binary/RelationalExpression.h"

namespace thogcc::ast::expressions::binary {

RelationalExpression::RelationalExpression(std::unique_ptr<ExpressionBase> lhs,
                                           std::unique_ptr<ExpressionBase> rhs,
                                           RelationalExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
