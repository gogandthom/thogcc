#pragma once

#include <memory>

#include "ast/expressions/ConstantExpression.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class SwitchCaseStatement : public StatementBase {
   public:
    SwitchCaseStatement(std::unique_ptr<expressions::ConstantExpression> expr,
                        std::unique_ptr<StatementBase> statement);
    SwitchCaseStatement(std::unique_ptr<StatementBase> statement);

   private:
    std::unique_ptr<expressions::ConstantExpression> _expr;
    std::unique_ptr<StatementBase> _statement;
    bool _isDefault;
};

};  // namespace thogcc::ast::statements
