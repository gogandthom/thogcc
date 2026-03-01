#pragma once

#include <memory>

#include "ast/DeclarationSpecifiers.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarations {

class ParameterDeclaration : public DeclarationBase {
   public:
    ParameterDeclaration(std::unique_ptr<DeclarationSpecifiers> specifiers,
                         std::unique_ptr<declarators::DeclaratorBase> declarator = nullptr);

   private:
    std::unique_ptr<DeclarationSpecifiers> specifiers;
    std::unique_ptr<declarators::DeclaratorBase> declarator;
};

}  // namespace thogcc::ast::declarations
