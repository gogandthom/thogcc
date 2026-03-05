#pragma once

#include <memory>
#include <utility>

#include "ast/DeclarationSpecifiers.h"
#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarations {

class Declaration : public DeclarationBase {
   public:
    Declaration(std::unique_ptr<DeclarationSpecifiers> specifiers,
                std::unique_ptr<NodeList<declarators::DeclaratorBase>> initDeclarators = nullptr)
        : _specifiers(std::move(specifiers)), _initDeclarators(std::move(initDeclarators)){};

   private:
    std::unique_ptr<DeclarationSpecifiers> _specifiers;
    std::unique_ptr<NodeList<declarators::DeclaratorBase>> _initDeclarators;
};

}  // namespace thogcc::ast::declarations
