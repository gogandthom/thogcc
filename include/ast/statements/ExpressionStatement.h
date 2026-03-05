#pragma once

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class ExpressionStatement : public StatementBase, public expressions::ExpressionBase {
   public:
    ExpressionStatement(std::unique_ptr<expressions::ExpressionBase> expr = nullptr)
        : _expr(std::move(expr)){};

   private:
    std::unique_ptr<expressions::ExpressionBase> _expr;
};

}  // namespace thogcc::ast::statements
