#pragma once

#include <memory>
#include <variant>

#include "ast/TypeName.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

class SizeofExpression : public ExpressionBase {
   public:
    SizeofExpression(std::unique_ptr<ExpressionBase> expr);
    SizeofExpression(std::unique_ptr<TypeName> typeName);

   private:
    std::variant<std::unique_ptr<ExpressionBase>, std::unique_ptr<TypeName>> _expr;
};

}  // namespace thogcc::ast::expressions::prefix
