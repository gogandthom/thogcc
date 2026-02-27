#include "ast/expressions/postfix/MemberAccessExpression.h"

namespace thogcc::ast::expressions::postfix {

MemberAccessExpression::MemberAccessExpression(std::unique_ptr<ExpressionBase> expr, bool exprIsPtr)
    : _expr(std::move(expr)), _exprIsPtr(exprIsPtr){};

}
