#include "ast/expressions/binary/BinaryExpressionBase.h"

namespace thogcc::ast::expressions::binary {

BinaryExpressionBase::BinaryExpressionBase(std::unique_ptr<ExpressionBase> lhs,
                                           std::unique_ptr<ExpressionBase> rhs)
    : _lhs(std::move(lhs)), _rhs(std::move(rhs)){};

}
