#pragma once

#include <memory>

#include "ast/expressions/ConstantExpression.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class SwitchCaseStatement : public StatementBase {
   public:
    SwitchCaseStatement(std::unique_ptr<expressions::ConstantExpression> expr,
                        std::unique_ptr<StatementBase> statement)
        : _expr(std::move(expr)), _statement(std::move(statement)), _isDefault(false){};
    SwitchCaseStatement(std::unique_ptr<StatementBase> statement)
        : _expr(nullptr), _statement(std::move(statement)), _isDefault(true){};

   private:
    std::unique_ptr<expressions::ConstantExpression> _expr;
    std::unique_ptr<StatementBase> _statement;
    bool _isDefault;
};

};  // namespace thogcc::ast::statements
