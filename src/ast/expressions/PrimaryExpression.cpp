#include "ast/expressions/PrimaryExpression.h"

namespace thogcc::ast::expressions {

template <typename T>
PrimaryExpression::PrimaryExpression(T&& value) : _value(std::forward<T>(value)){};

}
