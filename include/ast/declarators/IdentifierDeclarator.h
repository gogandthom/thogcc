#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

class IdentifierDeclarator : public VisitableNode<IdentifierDeclarator, DeclaratorBase> {
   public:
    IdentifierDeclarator(std::string identifier) : _identifier(std::move(identifier)){};

    std::string_view getIdentifier() const {
        return _identifier;
    }

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::declarators
