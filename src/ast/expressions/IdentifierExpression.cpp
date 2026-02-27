#include "ast/expressions/IdentifierExpression.h"

namespace thogcc::ast::expressions {

IdentifierExpression::IdentifierExpression(std::string identifier)
    : _identifier(std::move(identifier)){};

}
