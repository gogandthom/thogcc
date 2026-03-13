#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ConstantExpression : public VisitableNode<ConstantExpression, ExpressionBase> {
   public:
    ConstantExpression(std::unique_ptr<ExpressionBase> expr) : _expr(std::move(expr)){};

    auto* getExpr() const {
        return _expr.get();
    }

   private:
    std::unique_ptr<ExpressionBase> _expr;
};

}  // namespace thogcc::ast::expressions
