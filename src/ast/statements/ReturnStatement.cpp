#include "ast/statements/ReturnStatement.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::statements {

ReturnStatement::ReturnStatement(std::unique_ptr<expressions::ExpressionBase> expr)
    : _expr(std::move(expr)){};

}
