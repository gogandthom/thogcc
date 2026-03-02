#include "ast/expressions/postfix/ArrayAccessExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

ArrayAccessExpression::ArrayAccessExpression(std::unique_ptr<ExpressionBase> expr,
                                             std::unique_ptr<ExpressionBase> arrayExpr)
    : _expr(std::move(expr)), _arrayExpr(std::move(arrayExpr)){};

}
