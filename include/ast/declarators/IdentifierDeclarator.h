#pragma once

#include <string>
#include <utility>

#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

class IdentifierDeclarator : public DeclaratorBase {
   public:
    IdentifierDeclarator(std::string identifier) : _identifier(std::move(identifier)){};

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::declarators
