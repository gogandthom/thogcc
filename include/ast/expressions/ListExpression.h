#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ListExpression : public VisitableNode<ListExpression, ExpressionBase> {
   public:
    ListExpression(std::unique_ptr<ExpressionBase> expr)
        : _list(std::make_unique<NodeList<ExpressionBase>>(std::move(expr))){};
    void pushBack(std::unique_ptr<ExpressionBase> ptr) {
        _list->pushBack(std::move(ptr));
    };

    auto* getList() const {
        return _list.get();
    }

   private:
    std::unique_ptr<NodeList<ExpressionBase>> _list;
};

}  // namespace thogcc::ast::expressions
