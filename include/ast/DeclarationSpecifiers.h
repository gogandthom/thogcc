#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/StorageClassSpecifier.h"
#include "ast/TypeQualifier.h"

namespace thogcc::ast {

class DeclarationSpecifiers : public VisitableNode<DeclarationSpecifiers> {
   public:
    DeclarationSpecifiers(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier,
                          std::unique_ptr<Node> typeSpecifier)
        : _storageClassSpecifiers(std::make_unique<NodeList<ValueNode<StorageClassSpecifier>>>()),
          _typeSpecifiers(std::make_unique<NodeList<Node>>()) {
        if (storageClassSpecifier) {
            _storageClassSpecifiers->pushBack(std::move(storageClassSpecifier));
        }
        if (typeSpecifier) {
            _typeSpecifiers->pushBack(std::move(typeSpecifier));
        }
    };

    void pushBackStorage(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier) {
        _storageClassSpecifiers->pushBack(std::move(storageClassSpecifier));
    };

    void pushBackTypeSpecifier(std::unique_ptr<Node> typeSpecifier) {
        _typeSpecifiers->pushBack(std::move(typeSpecifier));
    };

    void pushBackTypeQualifier(std::unique_ptr<ValueNode<TypeQualifier>> typeQualifier) {
        _typeQualifiers->pushBack(std::move(typeQualifier));
    }

    bool isTypedef() const {
        if (!_storageClassSpecifiers) return false;
        for (const auto& specifier : _storageClassSpecifiers->getNodes()) {
            if (specifier->getValue() == StorageClassSpecifier::TYPEDEF) return true;
        }
        return false;
    };

    auto* getStorageClassSpecifier() const {
        return _storageClassSpecifiers.get();
    };
    auto* getTypeSpecifier() const {
        return _typeSpecifiers.get();
    };

    auto* getTypeQualifier() const {
        return _typeQualifiers.get();
    }

   private:
    std::unique_ptr<NodeList<ValueNode<StorageClassSpecifier>>> _storageClassSpecifiers;
    std::unique_ptr<NodeList<Node>> _typeSpecifiers;
    std::unique_ptr<NodeList<ValueNode<TypeQualifier>>> _typeQualifiers;
};

}  // namespace thogcc::ast
