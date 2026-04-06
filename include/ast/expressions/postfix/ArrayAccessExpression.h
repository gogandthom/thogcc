#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class ArrayAccessExpression : public VisitableNode<ArrayAccessExpression, ExpressionBase> {
   public:
    ArrayAccessExpression(std::unique_ptr<ExpressionBase> array,
                          std::unique_ptr<ExpressionBase> index)
        : _array(std::move(array)), _index(std::move(index)) {}

    auto* getArray() const {
        return _array.get();
    }
    auto* getIndex() const {
        return _index.get();
    }

   private:
    std::unique_ptr<ExpressionBase> _array, _index;
};

}  // namespace thogcc::ast::expressions::postfix
