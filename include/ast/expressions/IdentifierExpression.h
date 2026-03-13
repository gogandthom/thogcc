#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class IdentifierExpression : public VisitableNode<IdentifierExpression, ExpressionBase> {
   public:
    IdentifierExpression(std::string identifier) : _identifier(std::move(identifier)){};

    std::string_view getIdentifier() const {
        return _identifier;
    }

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::expressions
