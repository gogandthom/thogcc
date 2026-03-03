#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ListExpression : public ExpressionBase {
   public:
    ListExpression(std::unique_ptr<ExpressionBase> expr)
        : _list(std::make_unique<NodeList<ExpressionBase>>(std::move(expr))){};
    void pushBack(std::unique_ptr<ExpressionBase> ptr) {
        _list->pushBack(std::move(ptr));
    };

   private:
    std::unique_ptr<NodeList<ExpressionBase>> _list;
};

}  // namespace thogcc::ast::expressions
