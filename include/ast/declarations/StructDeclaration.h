#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/StructMemberDeclarator.h"

namespace thogcc::ast::declarations {

class StructDeclaration : public DeclarationBase {
   public:
    StructDeclaration(std::unique_ptr<NodeList<Node>> qualifiers,
                      std::unique_ptr<NodeList<declarators::StructMemberDeclarator>> declarators);

   private:
    std::unique_ptr<NodeList<Node>> qualifiers;
    std::unique_ptr<NodeList<declarators::StructMemberDeclarator>> declarators;
};

}  // namespace thogcc::ast::declarations
