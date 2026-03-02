#pragma once

#include <memory>

#include "ast/declarations/ParameterDeclaration.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/declarators/IdentifierDeclarator.h"

namespace thogcc::ast::declarators {

class FunctionDeclarator : public DeclaratorBase {
   public:
    FunctionDeclarator(
        std::unique_ptr<DeclaratorBase> base = nullptr,
        std::unique_ptr<NodeList<declarations::ParameterDeclaration>> params = nullptr);
    FunctionDeclarator(std::unique_ptr<DeclaratorBase> base,
                       std::unique_ptr<NodeList<IdentifierDeclarator>> identifiers = nullptr);

   private:
    std::unique_ptr<DeclaratorBase> _base;
    std::unique_ptr<NodeList<declarations::ParameterDeclaration>> _params;
    std::unique_ptr<NodeList<IdentifierDeclarator>> _identifiers;
};

}  // namespace thogcc::ast::declarators
