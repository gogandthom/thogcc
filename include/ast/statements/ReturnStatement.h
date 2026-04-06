#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class ReturnStatement : public VisitableNode<ReturnStatement, StatementBase> {
   public:
    ReturnStatement(std::unique_ptr<expressions::ExpressionBase> expr = nullptr)
        : _expr(std::move(expr)) {}

    auto* getExpr() const {
        return _expr.get();
    }

   private:
    std::unique_ptr<expressions::ExpressionBase> _expr;
};

}  // namespace thogcc::ast::statements
