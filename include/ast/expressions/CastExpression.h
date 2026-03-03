#pragma once

#include <memory>

#include "ast/TypeName.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class CastExpression : public ExpressionBase {
   public:
    CastExpression(std::unique_ptr<TypeName> typeName, std::unique_ptr<ExpressionBase> expr)
        : _expr(std::move(expr)), _typeName(std::move(typeName)){};

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::unique_ptr<TypeName> _typeName;
};

}  // namespace thogcc::ast::expressions
