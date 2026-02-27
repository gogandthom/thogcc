#include "ast/statements/CompoundStatement.h"

#include <memory>
#include <utility>

namespace thogcc::ast::statements {

CompoundStatement::CompoundStatement(
    std::unique_ptr<NodeList<declarations::DeclarationBase>> declarationList,
    std::unique_ptr<NodeList<StatementBase>> statementList)
    : _declarationList(std::move(declarationList)), _statementList(std::move(statementList)){};

}
