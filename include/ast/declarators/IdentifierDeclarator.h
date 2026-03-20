#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"
#include "types/Scope.h"

namespace thogcc::ast::declarators {

class IdentifierDeclarator : public VisitableNode<IdentifierDeclarator, DeclaratorBase> {
   public:
    IdentifierDeclarator(std::string identifier) : _identifier(std::move(identifier)){};

    std::string_view getIdentifier() const override {
        return _identifier;
    }

    void setSymbol(types::OrdSymbol symb) {
        _symb = std::move(symb);
    }
    const auto& getSymbol() const {
        return _symb;
    }

   private:
    std::string _identifier;
    types::OrdSymbol _symb;
};

}  // namespace thogcc::ast::declarators
