#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class EqualityExpression : public BinaryExpressionBase {
   public:
    EqualityExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                       bool isNe);

   private:
    bool _isNe;
};

}  // namespace thogcc::ast::expressions::binary
