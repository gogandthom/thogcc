#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/TypeQualifier.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

class PointerDeclarator : public VisitableNode<PointerDeclarator, DeclaratorBase> {
   public:
    PointerDeclarator(std::unique_ptr<NodeList<ValueNode<TypeQualifier>>> typeQualifiers = nullptr,
                      std::unique_ptr<DeclaratorBase> ptr = nullptr)
        : _ptr(std::move(ptr)), _typeQualifiers(std::move(typeQualifiers)){};
    /// Attach node to leaf
    void attach(std::unique_ptr<DeclaratorBase> node);

    std::string_view getIdentifier() const override {
        if (!_ptr) {
            throw std::runtime_error("getIndetifier() called on unattached PointerDeclarator.");
        }
        return _ptr->getIdentifier();
    };

    auto* getPtr() const {
        return _ptr.get();
    }

   private:
    std::unique_ptr<DeclaratorBase> _ptr;
    std::unique_ptr<NodeList<ValueNode<TypeQualifier>>> _typeQualifiers;
};

}  // namespace thogcc::ast::declarators
