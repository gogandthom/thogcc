#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::postfix {

class FunctionCallExpression : public VisitableNode<FunctionCallExpression, ExpressionBase> {
   public:
    FunctionCallExpression(std::unique_ptr<ExpressionBase> expr,
                           std::unique_ptr<NodeList<ExpressionBase>> args = nullptr)
        : _expr(std::move(expr)), _args(std::move(args)){};

   private:
    std::unique_ptr<ExpressionBase> _expr;
    std::unique_ptr<NodeList<ExpressionBase>> _args;
};

}  // namespace thogcc::ast::expressions::postfix
