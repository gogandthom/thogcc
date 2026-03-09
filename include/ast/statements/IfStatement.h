#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class IfStatement : public VisitableNode<IfStatement, StatementBase> {
   public:
    IfStatement(std::unique_ptr<expressions::ExpressionBase> expr,
                std::unique_ptr<StatementBase> ifStatement,
                std::unique_ptr<StatementBase> elseStatement = nullptr)
        : _expr(std::move(expr)),
          _ifStatement(std::move(ifStatement)),
          _elseStatement(std::move(elseStatement)){};

   private:
    std::unique_ptr<expressions::ExpressionBase> _expr;
    std::unique_ptr<StatementBase> _ifStatement;
    std::unique_ptr<StatementBase> _elseStatement;
};

}  // namespace thogcc::ast::statements
