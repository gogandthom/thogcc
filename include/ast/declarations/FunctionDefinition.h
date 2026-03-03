#pragma once

#include <memory>
#include <utility>

#include "ast/DeclarationSpecifiers.h"
#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/statements/CompoundStatement.h"

namespace thogcc::ast::declarations {

/// Function definition (ie. with CompoundStatement) only.
/// Function declarations do not have their own type.
class FunctionDefinition
    : public DeclarationBase {  // Must derive from DeclarationBase for external_declaration
   public:
    FunctionDefinition(std::unique_ptr<DeclarationSpecifiers> specifiers,
                       std::unique_ptr<declarators::DeclaratorBase> declarator,
                       std::unique_ptr<NodeList<DeclarationBase>> declarations,
                       std::unique_ptr<statements::CompoundStatement> statement)
        : _specifiers(std::move(specifiers)),
          _declarator(std::move(declarator)),
          _declarations(std::move(declarations)),
          _statement(std::move(statement)){};

   private:
    std::unique_ptr<DeclarationSpecifiers> _specifiers;
    std::unique_ptr<declarators::DeclaratorBase> _declarator;
    std::unique_ptr<NodeList<DeclarationBase>> _declarations;
    std::unique_ptr<statements::CompoundStatement> _statement;
};

}  // namespace thogcc::ast::declarations
