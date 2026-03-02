#include "ast/expressions/ConstantExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

ConstantExpression::ConstantExpression(std::unique_ptr<ExpressionBase> expr)
    : _expr(std::move(expr)){};

}
