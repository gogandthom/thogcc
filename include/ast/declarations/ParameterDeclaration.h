#pragma once

#include <memory>
#include <utility>

#include "ast/DeclarationSpecifiers.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarations {

class ParameterDeclaration : public DeclarationBase {
   public:
    ParameterDeclaration(std::unique_ptr<DeclarationSpecifiers> specifiers,
                         std::unique_ptr<declarators::DeclaratorBase> decl = nullptr)
        : _specifiers(std::move(specifiers)), _decl(std::move(decl)){};

   private:
    std::unique_ptr<DeclarationSpecifiers> _specifiers;
    std::unique_ptr<declarators::DeclaratorBase> _decl;
};

}  // namespace thogcc::ast::declarations
