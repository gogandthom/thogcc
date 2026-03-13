#pragma once

#include <string>
#include <variant>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"

namespace thogcc::ast::expressions {

class PrimaryExpression : public VisitableNode<PrimaryExpression, ExpressionBase> {
   public:
    template <typename T>
    PrimaryExpression(T&& value) : _value(std::forward<T>(value)){};

    auto& getValue() const {
        return _value;
    };

   private:
    std::variant<int, unsigned, long, unsigned long, float, double, std::string> _value;
};

}  // namespace thogcc::ast::expressions
