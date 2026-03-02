#include "ast/expressions/postfix/MemberAccessExpression.h"

#include <string>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

MemberAccessExpression::MemberAccessExpression(std::unique_ptr<ExpressionBase> expr,
                                               std::string identifier, bool exprIsPtr)
    : _expr(std::move(expr)), _identifier(std::move(identifier)), _exprIsPtr(exprIsPtr){};

}
