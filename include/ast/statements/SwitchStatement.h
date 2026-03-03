#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class SwitchStatement : public StatementBase {
   public:
    SwitchStatement(std::unique_ptr<expressions::ExpressionBase> expr,
                    std::unique_ptr<StatementBase> statement)
        : _expr(std::move(expr)), _statement(std::move(statement)){};

   private:
    std::unique_ptr<expressions::ExpressionBase> _expr;
    std::unique_ptr<StatementBase> _statement;
};

}  // namespace thogcc::ast::statements
