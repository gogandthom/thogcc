#include "ast/expressions/binary/EqualityExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

EqualityExpression::EqualityExpression(std::unique_ptr<ExpressionBase> lhs,
                                       std::unique_ptr<ExpressionBase> rhs, bool isNe)
    : BinaryExpressionBase(std::move(lhs), std::move(rhs)), _isNe(isNe){};

}
