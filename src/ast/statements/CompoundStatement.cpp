#include "ast/statements/CompoundStatement.h"

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

CompoundStatement::CompoundStatement(
    std::unique_ptr<NodeList<declarations::DeclarationBase>> declarationList,
    std::unique_ptr<NodeList<StatementBase>> statementList)
    : _declarationList(std::move(declarationList)), _statementList(std::move(statementList)){};

}
