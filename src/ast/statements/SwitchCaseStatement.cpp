#include "ast/statements/SwitchCaseStatement.h"

#include <memory>
#include <utility>

#include "ast/expressions/ConstantExpression.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

SwitchCaseStatement::SwitchCaseStatement(std::unique_ptr<expressions::ConstantExpression> expr,
                                         std::unique_ptr<StatementBase> statement)
    : _expr(std::move(expr)), _statement(std::move(statement)), _isDefault(false){};

SwitchCaseStatement::SwitchCaseStatement(std::unique_ptr<StatementBase> statement)
    : _expr(nullptr), _statement(std::move(statement)), _isDefault(true){};

}  // namespace thogcc::ast::statements
