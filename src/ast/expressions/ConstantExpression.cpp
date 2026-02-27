#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::expressions {

ConstantExpression::ConstantExpression(std::unique_ptr<ExpressionBase> expr)
    : _expr(std::move(expr)){};

}
