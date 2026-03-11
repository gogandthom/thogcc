#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ConditionalExpression : public VisitableNode<ConditionalExpression, ExpressionBase> {
   public:
    ConditionalExpression(std::unique_ptr<ExpressionBase> cond,
                          std::unique_ptr<ExpressionBase> ifExpr,
                          std::unique_ptr<ExpressionBase> elseExpr)
        : _cond(std::move(cond)), _ifExpr(std::move(ifExpr)), _elseExpr(std::move(elseExpr)){};

    auto* getCond() const {
        return _cond.get();
    }
    auto* getIfExpr() const {
        return _ifExpr.get();
    }
    auto* getElseExpr() const {
        return _elseExpr.get();
    }

   private:
    std::unique_ptr<ExpressionBase> _cond, _ifExpr, _elseExpr;
};

}  // namespace thogcc::ast::expressions
