#include "ast/expressions/IdentifierExpression.h"

#include <string>
#include <utility>

namespace thogcc::ast::expressions {

IdentifierExpression::IdentifierExpression(std::string identifier)
    : _identifier(std::move(identifier)){};

}
