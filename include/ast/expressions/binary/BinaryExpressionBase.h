#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class BinaryExpressionBase : public ExpressionBase {
   public:
    BinaryExpressionBase(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)){};

   private:
    std::unique_ptr<ExpressionBase> _lhs, _rhs;
};

}  // namespace thogcc::ast::expressions::binary
