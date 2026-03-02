#include "ast/expressions/binary/RelationalExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

RelationalExpression::RelationalExpression(std::unique_ptr<ExpressionBase> lhs,
                                           std::unique_ptr<ExpressionBase> rhs,
                                           RelationalExpressionType op)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _op(op){};

}
