#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/Initializer.h"
#include "types/Scope.h"

namespace thogcc::ast::declarators {

class InitDeclarator : public VisitableNode<InitDeclarator, DeclaratorBase> {
   public:
    InitDeclarator(std::unique_ptr<DeclaratorBase> decl,
                   std::unique_ptr<expressions::Initializer> initializer = nullptr)
        : _decl(std::move(decl)), _initializer(std::move(initializer)){};

    std::string_view getIdentifier() const override {
        return _decl->getIdentifier();
    };

    auto* getDecl() const {
        return _decl.get();
    }
    auto* getInitializer() const {
        return _initializer.get();
    }

    void setSymbol(types::OrdSymbol symb) {
        _symb = std::move(symb);
    }
    const auto& getSymbol() const {
        return _symb;
    }

   private:
    std::unique_ptr<DeclaratorBase> _decl;
    std::unique_ptr<expressions::Initializer> _initializer;
    types::OrdSymbol _symb;
};

}  // namespace thogcc::ast::declarators
