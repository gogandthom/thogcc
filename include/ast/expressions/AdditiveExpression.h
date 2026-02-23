#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class AdditiveExpression : public ExpressionBase {
   public:
    AdditiveExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                       bool isSub = false);

   private:
    std::unique_ptr<ExpressionBase> _lhs;
    std::unique_ptr<ExpressionBase> _rhs;
    bool _isSub;
};

}  // namespace thogcc::ast::expressions
