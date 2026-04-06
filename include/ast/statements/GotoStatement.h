#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class GotoStatement : public VisitableNode<GotoStatement, StatementBase> {
   public:
    GotoStatement(std::string identifier) : _identifier(std::move(identifier)) {}

    std::string_view getIdentifier() const {
        return _identifier;
    }

   private:
    std::string _identifier;
};

}  // namespace thogcc::ast::statements
