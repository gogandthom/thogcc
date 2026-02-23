#pragma once

#include <cstdint>
#include <memory>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

enum class AssignmentExpressionType : std::uint8_t {
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
};

class AssignmentExpression : public ExpressionBase {
   public:
    AssignmentExpression(std::unique_ptr<ExpressionBase> unaryExpr, AssignmentExpressionType type,
                         std::unique_ptr<ExpressionBase> assExpr);

   private:
    std::unique_ptr<ExpressionBase> _unaryExpr;
    std::unique_ptr<ExpressionBase> _assExpr;
    AssignmentExpressionType _type;
};

}  // namespace thogcc::ast::expressions
