#pragma once

#include "Node.h"
#include "ast/declarators/EnumValueDeclarator.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast {

class EnumSpecifier : public Node {
   public:
    EnumSpecifier(std::string identifier,
                  std::unique_ptr<NodeList<declarators::EnumValueDeclarator>> expr = nullptr);

   private:
    std::string _identifier;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast
