#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast {

class TypeName : public VisitableNode<TypeName> {
   public:
    TypeName(std::unique_ptr<NodeList<Node>> specifiers,
             std::unique_ptr<declarators::DeclaratorBase> declarators = nullptr)
        : _specifiers(std::move(specifiers)), _declarator(std::move(declarators)){};

    auto* getSpecifiers() const {
        return _specifiers.get();
    }
    auto* getDeclarator() const {
        return _declarator.get();
    }

   private:
    std::unique_ptr<NodeList<Node>> _specifiers;
    std::unique_ptr<declarators::DeclaratorBase> _declarator;
};

}  // namespace thogcc::ast
