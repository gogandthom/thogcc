#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class LabelledStatement : public VisitableNode<LabelledStatement, StatementBase> {
   public:
    LabelledStatement(std::string identifier, std::unique_ptr<StatementBase> statement)
        : _identifier(std::move(identifier)), _statement(std::move(statement)) {};

    auto* getStatement() const {
        return _statement.get();
    }

    std::string_view getIdentifier() const {
        return _identifier;
    }

   private:
    std::string _identifier;
    std::unique_ptr<StatementBase> _statement;
};

}  // namespace thogcc::ast::statements
