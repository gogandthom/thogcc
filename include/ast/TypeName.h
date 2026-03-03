#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast {

class TypeName : public Node {
   public:
    TypeName(std::unique_ptr<NodeList<Node>> specifiers,
             std::unique_ptr<declarators::DeclaratorBase> declarators = nullptr)
        : _specifiers(std::move(specifiers)), _declarator(std::move(declarators)){};

   private:
    std::unique_ptr<NodeList<Node>> _specifiers;
    std::unique_ptr<declarators::DeclaratorBase> _declarator;
};

}  // namespace thogcc::ast
