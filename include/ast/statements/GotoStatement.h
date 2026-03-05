#pragma once

#include <string>
#include <utility>

#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class GotoStatement : public StatementBase {
   public:
    GotoStatement(std::string identifier) : _identifier(std::move(identifier)){};

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::statements
