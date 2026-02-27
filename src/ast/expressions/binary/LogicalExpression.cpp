#include "ast/expressions/binary/LogicalExpression.h"

namespace thogcc::ast::expressions::binary {

LogicalExpression::LogicalExpression(std::unique_ptr<ExpressionBase> lhs,
                                     std::unique_ptr<ExpressionBase> rhs, LogicalExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
