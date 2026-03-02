#include "ast/expressions/ListExpression.h"

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

ListExpression::ListExpression(std::unique_ptr<ExpressionBase> expr)
    : _list(std::make_unique<NodeList<ExpressionBase>>(std::move(expr))){};

}
