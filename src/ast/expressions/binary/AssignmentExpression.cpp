#include "ast/expressions/binary/AssignmentExpression.h"

namespace thogcc::ast::expressions::binary {

AssignmentExpression::AssignmentExpression(std::unique_ptr<ExpressionBase> lhs,
                                           std::unique_ptr<ExpressionBase> rhs,
                                           AssignmentExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
