#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/StorageClassSpecifier.h"

namespace thogcc::ast {

class DeclarationSpecifiers : public Node {
   public:
    DeclarationSpecifiers(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier,
                          std::unique_ptr<Node> typeSpecifier)
        : _storageClassSpecifiers(std::make_unique<NodeList<ValueNode<StorageClassSpecifier>>>(
              std::move(storageClassSpecifier))),
          _typeSpecifiers(std::make_unique<NodeList<Node>>(std::move(typeSpecifier))){};
    void pushBackStorage(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier) {
        _storageClassSpecifiers->pushBack(std::move(storageClassSpecifier));
    };
    void pushBackType(std::unique_ptr<Node> typeSpecifier) {
        _typeSpecifiers->pushBack(std::move(typeSpecifier));
    };

   private:
    std::unique_ptr<NodeList<ValueNode<StorageClassSpecifier>>> _storageClassSpecifiers;
    std::unique_ptr<NodeList<Node>> _typeSpecifiers;
};

}  // namespace thogcc::ast
