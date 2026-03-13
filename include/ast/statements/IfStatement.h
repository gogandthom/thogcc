#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class IfStatement : public VisitableNode<IfStatement, StatementBase> {
   public:
    IfStatement(std::unique_ptr<expressions::ExpressionBase> cond,
                std::unique_ptr<StatementBase> ifStatement,
                std::unique_ptr<StatementBase> elseStatement = nullptr)
        : _cond(std::move(cond)),
          _ifStatement(std::move(ifStatement)),
          _elseStatement(std::move(elseStatement)) {};

    auto* getCond() const {
        return _cond.get();
    }
    auto* getIfStatement() const {
        return _ifStatement.get();
    }
    auto* getElseStatement() const {
        return _elseStatement.get();
    }

   private:
    std::unique_ptr<expressions::ExpressionBase> _cond;
    std::unique_ptr<StatementBase> _ifStatement;
    std::unique_ptr<StatementBase> _elseStatement;
};

}  // namespace thogcc::ast::statements
