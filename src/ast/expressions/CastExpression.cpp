#include "ast/expressions/CastExpression.h"

#include <memory>
#include <utility>

#include "ast/TypeName.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

CastExpression::CastExpression(std::unique_ptr<TypeName> typeName,
                               std::unique_ptr<ExpressionBase> expr)
    : _expr(std::move(expr)), _typeName(std::move(typeName)){};

}
