#pragma once

#include <memory>
#include <optional>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ConstantExpression : public VisitableNode<ConstantExpression, ExpressionBase> {
   public:
    ConstantExpression(std::unique_ptr<ExpressionBase> expr) : _expr(std::move(expr)) {}

    auto* getExpr() const {
        return _expr.get();
    }

    void setConstVal(auto val) {
        _constVal = val;
    }
    const auto& getConstVal() const {
        return _constVal;
    }

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::optional<std::variant<int, float, double>> _constVal;
};

}  // namespace thogcc::ast::expressions
