#pragma once

#include <utility>

#include "Node.h"
#include "ast/declarators/EnumValueDeclarator.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast {

class EnumSpecifier : public Node {
   public:
    EnumSpecifier(std::string identifier,
                  std::unique_ptr<NodeList<declarators::EnumValueDeclarator>> declarators = nullptr)
        : _identifier(std::move(identifier)), _declarators(std::move(declarators)){};

   private:
    std::string _identifier;
    std::unique_ptr<NodeList<declarators::EnumValueDeclarator>> _declarators;
};

}  // namespace thogcc::ast
