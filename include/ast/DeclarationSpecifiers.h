#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/StorageClassSpecifier.h"
#include "ast/TypeQualifier.h"

namespace thogcc::ast {

class DeclarationSpecifiers : public Node {
   public:
    DeclarationSpecifiers(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier,
                          std::unique_ptr<Node> typeSpecifier,
                          std::unique_ptr<ValueNode<TypeQualifier>> typeQualifier)
        : _storageClassSpecifiers(std::make_unique<NodeList<ValueNode<StorageClassSpecifier>>>(
              std::move(storageClassSpecifier))),
          _typeSpecifiers(std::make_unique<NodeList<Node>>(std::move(typeSpecifier))) {};

    void pushBackStorage(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier) {
        _storageClassSpecifiers->pushBack(std::move(storageClassSpecifier));
    };

    void pushBackTypeSpecifier(std::unique_ptr<Node> typeSpecifier) {
        _typeSpecifiers->pushBack(std::move(typeSpecifier));
    };

    void pushBackTypeQualifier(std::unique_ptr<ValueNode<TypeQualifier>> typeQualifier) {
        _typeQualifiers->pushBack(std::move(typeQualifier));
    }

   private:
    std::unique_ptr<NodeList<ValueNode<StorageClassSpecifier>>> _storageClassSpecifiers;
    std::unique_ptr<NodeList<Node>> _typeSpecifiers;
    std::unique_ptr<NodeList<ValueNode<TypeQualifier>>> _typeQualifiers;
};

}  // namespace thogcc::ast
