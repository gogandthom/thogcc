#include "ast/statements/IfStatement.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

IfStatement::IfStatement(std::unique_ptr<expressions::ExpressionBase> expr,
                         std::unique_ptr<StatementBase> ifStatement,
                         std::unique_ptr<StatementBase> elseStatement)
    : _expr(std::move(expr)),
      _ifStatement(std::move(ifStatement)),
      _elseStatement(std::move(elseStatement)){};

}
