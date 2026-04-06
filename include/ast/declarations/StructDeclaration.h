#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/declarations/DeclarationBase.h"
#include "ast/declarators/StructMemberDeclarator.h"

namespace thogcc::ast::declarations {

class StructDeclaration : public VisitableNode<StructDeclaration, DeclarationBase> {
   public:
    StructDeclaration(std::unique_ptr<NodeList<Node>> qualifiers,
                      std::unique_ptr<NodeList<declarators::StructMemberDeclarator>> declarators)
        : _qualifiers(std::move(qualifiers)), _declarators(std::move(declarators)) {}

    auto* getQualifiers() const {
        return _qualifiers.get();
    }
    auto* getDeclarators() const {
        return _declarators.get();
    }

   private:
    std::unique_ptr<NodeList<Node>> _qualifiers;
    std::unique_ptr<NodeList<declarators::StructMemberDeclarator>> _declarators;
};

}  // namespace thogcc::ast::declarations
