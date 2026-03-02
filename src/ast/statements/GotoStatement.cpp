#include "ast/statements/GotoStatement.h"

#include <string>
#include <utility>

namespace thogcc::ast::statements {

GotoStatement::GotoStatement(std::string identifier) : _identifier(std::move(identifier)){};

}
