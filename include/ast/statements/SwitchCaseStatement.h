#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ConstantExpression.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class SwitchCaseStatement : public VisitableNode<SwitchCaseStatement, StatementBase> {
   public:
    SwitchCaseStatement(std::unique_ptr<expressions::ConstantExpression> cond,
                        std::unique_ptr<StatementBase> statement)
        : _cond(std::move(cond)), _statement(std::move(statement)), _isDefault(false) {}
    SwitchCaseStatement(std::unique_ptr<StatementBase> statement)
        : _cond(nullptr), _statement(std::move(statement)), _isDefault(true) {}

    auto* getCond() const {
        return _cond.get();
    }
    auto* getStatement() const {
        return _statement.get();
    }

    bool getIsDefault() const {
        return _isDefault;
    }

   private:
    std::unique_ptr<expressions::ConstantExpression> _cond;
    std::unique_ptr<StatementBase> _statement;
    bool _isDefault;
};

};  // namespace thogcc::ast::statements
