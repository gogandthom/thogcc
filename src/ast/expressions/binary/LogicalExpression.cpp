#include "ast/expressions/binary/LogicalExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

LogicalExpression::LogicalExpression(std::unique_ptr<ExpressionBase> lhs,
                                     std::unique_ptr<ExpressionBase> rhs, LogicalExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
