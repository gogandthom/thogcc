#include "ast/expressions/prefix/SizeofExpression.h"

#include <memory>
#include <utility>

#include "ast/TypeName.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

SizeofExpression::SizeofExpression(std::unique_ptr<ExpressionBase> expr) : _expr(std::move(expr)){};

SizeofExpression::SizeofExpression(std::unique_ptr<TypeName> typeName)
    : _expr(std::move(typeName)){};

}  // namespace thogcc::ast::expressions::prefix
