#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class ReturnStatement : public StatementBase {
   public:
    ReturnStatement(std::unique_ptr<expressions::ExpressionBase> expr = nullptr)
        : _expr(std::move(expr)){};

   private:
    std::unique_ptr<expressions::ExpressionBase> _expr;
};

}  // namespace thogcc::ast::statements
