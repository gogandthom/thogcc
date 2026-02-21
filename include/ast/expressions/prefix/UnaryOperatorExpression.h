#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/ExpressionUnion.h"

namespace thogcc::ast::expressions::prefix {

typedef enum {
    ADDRESSOF,
    INDIRECTION,
    PLUS,
    MINUS,
    BITWISE_NOT,
    LOGICAL_NOT,
    SIZEOF,
} UnaryOperatorType;

class UnaryOperatorExpression : public ExpressionBase {
   public:
    UnaryOperatorExpression(UnaryOperatorType type, std::unique_ptr<ExpressionUnion> expr);
    UnaryOperatorExpression(UnaryOperatorType type /* TODO , type_name */);

   private:
    UnaryOperatorType _type;
    std::unique_ptr<ExpressionUnion> _expr;
};

}  // namespace thogcc::ast::expressions::prefix
