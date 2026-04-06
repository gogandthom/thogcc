#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::binary {

class BinaryExpressionBase : public VisitableNode<BinaryExpressionBase, ExpressionBase> {
   public:
    BinaryExpressionBase(std::unique_ptr<ExpressionBase> lhs, std::unique_ptr<ExpressionBase> rhs)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)) {}

    auto* getLhs() const {
        return _lhs.get();
    }
    auto* getRhs() const {
        return _rhs.get();
    }

   private:
    std::unique_ptr<ExpressionBase> _lhs, _rhs;
};

}  // namespace thogcc::ast::expressions::binary
