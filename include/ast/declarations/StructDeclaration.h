#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/StructDeclarator.h"

namespace thogcc::ast::declarations {

class StructDeclaration : public DeclarationBase {
   public:
    StructDeclaration(std::unique_ptr<NodeList<Node>> qualifiers,
                      std::unique_ptr<NodeList<declarators::StructDeclarator>> declarators);

   private:
    std::unique_ptr<NodeList<Node>> qualifiers;
    std::unique_ptr<NodeList<declarators::StructDeclarator>> declarators;
};

}  // namespace thogcc::ast::declarations
