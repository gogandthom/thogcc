#pragma once

#include <memory>

#include "ast/expressions/ExpressionBase.h"
#include "ast/expressions/ExpressionUnion.h"

namespace thogcc::ast::expressions {

typedef enum {
    ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    LEFT_ASSIGN,
    RIGHT_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    OR_ASSIGN,
} AssignmentExpressionType;

class AssignmentExpression : public ExpressionBase {
   public:
    AssignmentExpression(std::unique_ptr<ExpressionUnion> unaryExpr, AssignmentExpressionType type,
                         std::unique_ptr<ExpressionUnion> assExpr);

   private:
    std::unique_ptr<ExpressionUnion> _unaryExpr;
    std::unique_ptr<ExpressionUnion> _assExpr;
    AssignmentExpressionType _type;
};

}  // namespace thogcc::ast::expressions
