#pragma once

#include <memory>
#include <utility>

#include "ast/DeclarationSpecifiers.h"
#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/DeclaratorBase.h"
#include "types/Scope.h"

namespace thogcc::ast::declarations {

class ParameterDeclaration : public VisitableNode<ParameterDeclaration, DeclarationBase> {
   public:
    ParameterDeclaration(std::unique_ptr<DeclarationSpecifiers> specifiers,
                         std::unique_ptr<declarators::DeclaratorBase> decl = nullptr)
        : _specifiers(std::move(specifiers)), _decl(std::move(decl)){};

    auto* getSpecifiers() const {
        return _specifiers.get();
    }
    auto* getDecl() const {
        return _decl.get();
    }

    void setSymbol(std::shared_ptr<types::VarSymbol> symb) {
        _symb = std::move(symb);
    }
    auto getSymbol() const {
        return _symb;
    }

   private:
    std::unique_ptr<DeclarationSpecifiers> _specifiers;
    std::unique_ptr<declarators::DeclaratorBase> _decl;
    std::shared_ptr<types::VarSymbol> _symb;
};

}  // namespace thogcc::ast::declarations
