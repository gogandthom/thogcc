#pragma once

#include <memory>
#include <string>

#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class LabelledStatement : public StatementBase {
   public:
    LabelledStatement(std::string identifier, std::unique_ptr<StatementBase> statement);

   private:
    std::string _identifier;
    std::unique_ptr<StatementBase> _statement;
};

}  // namespace thogcc::ast::statements
