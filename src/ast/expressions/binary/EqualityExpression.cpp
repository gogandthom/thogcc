#include "ast/expressions/binary/EqualityExpression.h"

namespace thogcc::ast::expressions::binary {

EqualityExpression::EqualityExpression(std::unique_ptr<ExpressionBase> lhs,
                                       std::unique_ptr<ExpressionBase> rhs, bool isNe)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _isNe(isNe){};

}
