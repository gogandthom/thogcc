#include "ast/statements/ExpressionStatement.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::statements {

Expressiontatement::Expressiontatement(std::unique_ptr<expressions::ExpressionBase> expr)
    : _expr(std::move(expr)){};

}
