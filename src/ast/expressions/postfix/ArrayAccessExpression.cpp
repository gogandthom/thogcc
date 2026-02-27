#include "ast/expressions/postfix/ArrayAccessExpression.h"

namespace thogcc::ast::expressions::postfix {

ArrayAccessExpression::ArrayAccessExpression(std::unique_ptr<ExpressionBase> expr,
                                             std::unique_ptr<ExpressionBase> arrayExpr)
    : _expr(std::move(expr)), _arrayExpr(std::move(arrayExpr)){};

}
