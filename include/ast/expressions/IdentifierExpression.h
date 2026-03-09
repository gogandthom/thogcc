#pragma once

#include <string>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class IdentifierExpression : public VisitableNode<IdentifierExpression, ExpressionBase> {
   public:
    IdentifierExpression(std::string identifier) : _identifier(std::move(identifier)){};

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::expressions
