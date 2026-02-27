#include "ast/expressions/binary/AddMultExpression.h"

namespace thogcc::ast::expressions::binary {

AddMultExpression::AddMultExpression(std::unique_ptr<ExpressionBase> lhs,
                                     std::unique_ptr<ExpressionBase> rhs, AddMultExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
