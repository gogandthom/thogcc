#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/TypeName.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class CastExpression : public VisitableNode<CastExpression, ExpressionBase> {
   public:
    CastExpression(std::unique_ptr<TypeName> typeName, std::unique_ptr<ExpressionBase> expr)
        : _expr(std::move(expr)), _typeName(std::move(typeName)) {}

    auto* getExpr() const {
        return _expr.get();
    }
    auto* getTypeName() const {
        return _typeName.get();
    }

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::unique_ptr<TypeName> _typeName;
};

}  // namespace thogcc::ast::expressions
