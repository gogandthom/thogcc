#pragma once

#include <memory>
#include <utility>
#include <variant>

#include "ast/TypeName.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

class SizeofExpression : public ExpressionBase {
   public:
    SizeofExpression(std::unique_ptr<ExpressionBase> expr) : _expr(std::move(expr)){};
    SizeofExpression(std::unique_ptr<TypeName> typeName) : _expr(std::move(typeName)){};

   private:
    std::variant<std::unique_ptr<ExpressionBase>, std::unique_ptr<TypeName>> _expr;
};

}  // namespace thogcc::ast::expressions::prefix
