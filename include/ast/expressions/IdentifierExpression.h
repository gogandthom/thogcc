#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "types/Scope.h"

namespace thogcc::ast::expressions {

class IdentifierExpression : public VisitableNode<IdentifierExpression, ExpressionBase> {
   public:
    IdentifierExpression(std::string identifier) : _identifier(std::move(identifier)){};

    std::string_view getIdentifier() const {
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

}  // namespace thogcc::ast::expressions
