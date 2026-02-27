#include "ast/statements/LabelledStatement.h"

#include <memory>
#include <utility>

namespace thogcc::ast::statements {

LabelledStatement::LabelledStatement(std::string identifier,
                                     std::unique_ptr<StatementBase> statement)
    : _identifier(std::move(identifier)), _statement(std::move(statement)){};

}
