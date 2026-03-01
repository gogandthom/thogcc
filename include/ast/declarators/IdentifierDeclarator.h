#pragma once

#include <string>

#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

class IdentifierDeclarator : public DeclaratorBase {
   public:
    IdentifierDeclarator(std::string identifier);

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::declarators
