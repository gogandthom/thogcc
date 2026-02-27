#include "ast/expressions/prefix/SizeofExpression.h"

namespace thogcc::ast::expressions::prefix {

SizeofExpression::SizeofExpression(std::unique_ptr<ExpressionBase> expr) : _expr(std::move(expr)){};

SizeofExpression::SizeofExpression(std::unique_ptr<TypeName> typeName)
    : _expr(std::move(typeName)){};

}