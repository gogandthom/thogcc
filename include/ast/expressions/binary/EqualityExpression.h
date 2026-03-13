#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class EqualityExpression : public VisitableNode<EqualityExpression, BinaryExpressionBase> {
   public:
    EqualityExpression(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs,
                       bool isNe = false)
        : VisitableNode(std::move(lhs), std::move(rhs)),  // Must call direct parent's constructor
          _isNe(isNe) {};

    bool getIsNe() {
        return _isNe;
    }

   private:
    bool _isNe;
};

}  // namespace thogcc::ast::expressions::binary
