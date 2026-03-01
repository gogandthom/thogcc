#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/StorageClassSpecifier.h"

namespace thogcc::ast {

class DeclarationSpecifiers : public Node {
   public:
    DeclarationSpecifiers(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier,
                          std::unique_ptr<Node> typeSpecifier);
    void pushBackStorage(std::unique_ptr<ValueNode<StorageClassSpecifier>> storageClassSpecifier);
    void pushBackType(std::unique_ptr<Node> typeSpecifier);

   private:
    std::unique_ptr<ValueNode<StorageClassSpecifier>> _storageClassSpecifier;
    std::unique_ptr<Node> _typeSpecifier;
};

}  // namespace thogcc::ast
