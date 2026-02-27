#include "ast/expressions/postfix/FunctionCallExpression.h"

namespace thogcc::ast::expressions::postfix {

FunctionCallExpression::FunctionCallExpression(std::unique_ptr<ExpressionBase> expr,
                                               std::unique_ptr<NodeList<ExpressionBase>> args)
    : _expr(std::move(expr)), _args(std::move(args)){};

}
