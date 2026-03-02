#include "ast/expressions/binary/AddMultExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

AddMultExpression::AddMultExpression(std::unique_ptr<ExpressionBase> lhs,
                                     std::unique_ptr<ExpressionBase> rhs, AddMultExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
