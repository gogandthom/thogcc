#include "ast/expressions/IncDecExpression.h"

namespace thogcc::ast::expressions {

IncDecExpression::IncDecExpression(std::unique_ptr<ExpressionBase> expr, bool isDecrement,
                                   bool isPrefix)
    : _expr(std::move(expr)), _isDecrement(isDecrement), _isPrefix(isPrefix){};

}
