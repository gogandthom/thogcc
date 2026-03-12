#pragma once

#include <memory>
#include <utility>

#include "ast/TypeQualifier.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

class PointerDeclarator : public DeclaratorBase {
   public:
    PointerDeclarator(std::unique_ptr<NodeList<ValueNode<TypeQualifier>>> typeQualifiers = nullptr,
                      std::unique_ptr<DeclaratorBase> ptr = nullptr)
        : _ptr(std::move(ptr)), _typeQualifiers(std::move(typeQualifiers)) {};
    /// Attach node to leaf
    void attach(std::unique_ptr<DeclaratorBase> node);

   private:
    std::unique_ptr<DeclaratorBase> _ptr;
    std::unique_ptr<NodeList<ValueNode<TypeQualifier>>> _typeQualifiers;
};

}  // namespace thogcc::ast::declarators
