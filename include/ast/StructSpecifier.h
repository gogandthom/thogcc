#pragma once

#include <memory>
#include <string>

#include "ast/Node.h"
#include "ast/declarations/StructDeclaration.h"

namespace thogcc::ast {

class StructSpecifier : public Node {
   public:
    StructSpecifier(std::unique_ptr<NodeList<declarations::StructDeclaration>> declarations,
                    std::string identifier = nullptr);

   private:
    std::unique_ptr<NodeList<declarations::StructDeclaration>> declarations;
    std::string identifier;
};

}  // namespace thogcc::ast
