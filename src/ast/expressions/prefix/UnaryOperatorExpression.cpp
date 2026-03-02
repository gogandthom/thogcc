#include "ast/expressions/prefix/UnaryOperatorExpression.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions::prefix {

UnaryOperatorExpression::UnaryOperatorExpression(UnaryOperatorType op,
                                                 std::unique_ptr<ExpressionBase> expr)
    : _op(std::move(op)), _expr(std::move(expr)){};

}
