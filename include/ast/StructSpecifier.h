#pragma once

#include <memory>
#include <string>
#include <utility>

#include "ast/Node.h"
#include "ast/declarations/StructDeclaration.h"

namespace thogcc::ast {

class StructSpecifier : public VisitableNode<StructSpecifier> {
   public:
    StructSpecifier(std::unique_ptr<NodeList<declarations::StructDeclaration>> declarations,
                    std::string identifier = {})
        : _declarations(std::move(declarations)), _identifier(std::move(identifier)){};

   private:
    std::unique_ptr<NodeList<declarations::StructDeclaration>> _declarations;
    std::string _identifier;
};

}  // namespace thogcc::ast
