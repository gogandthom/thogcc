#pragma once

#include <memory>

#include "ast/declarations/DeclarationBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class CompoundStatement : public StatementBase {
   public:
    CompoundStatement(
        std::unique_ptr<NodeList<declarations::DeclarationBase>> declarationList = nullptr,
        std::unique_ptr<NodeList<StatementBase>> statementList = nullptr)
        : _declarationList(std::move(declarationList)), _statementList(std::move(statementList)){};

   private:
    std::unique_ptr<NodeList<declarations::DeclarationBase>> _declarationList;
    std::unique_ptr<NodeList<StatementBase>> _statementList;
};

}  // namespace thogcc::ast::statements
