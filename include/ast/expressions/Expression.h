#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/expressions/AssignmentExpression.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class Expression : public ExpressionBase {
   public:
    Expression(std::unique_ptr<NodeList<AssignmentExpression>> assExprList);

   private:
    std::unique_ptr<NodeList<AssignmentExpression>> _assExprList;
};

}  // namespace thogcc::ast::expressions
