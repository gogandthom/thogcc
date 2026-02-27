#include "ast/statements/SwitchStatement.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

SwitchStatement::SwitchStatement(std::unique_ptr<expressions::ExpressionBase> expr,
                                 std::unique_ptr<StatementBase> statement)
    : _expr(std::move(expr)), _statement(std::move(statement)){};

}
