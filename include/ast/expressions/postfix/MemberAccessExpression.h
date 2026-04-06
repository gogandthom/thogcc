#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class MemberAccessExpression : public VisitableNode<MemberAccessExpression, ExpressionBase> {
   public:
    MemberAccessExpression(std::unique_ptr<ExpressionBase> expr, std::string identifier,
                           bool exprIsPtr = false)
        : _expr(std::move(expr)), _identifier(std::move(identifier)), _exprIsPtr(exprIsPtr) {}

    auto* getExpr() const {
        return _expr.get();
    }

    std::string_view getIdentifier() const {
        return _identifier;
    }

    bool getExprIsPtr() const {
        return _exprIsPtr;
    }

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::string _identifier;
    bool _exprIsPtr;
};

}  // namespace thogcc::ast::expressions::postfix
