#pragma once

#include <memory>
#include <vector>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class ListExpression : public ExpressionBase {
   public:
    ListExpression(std::unique_ptr<ExpressionBase> expr);
    std::unique_ptr<ListExpression> pushBack(std::unique_ptr<ExpressionBase> ptr);

   private:
    std::vector<std::unique_ptr<ExpressionBase>> _list;
};

}  // namespace thogcc::ast::expressions
