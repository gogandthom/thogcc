#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class EnumValueDeclarator : public VisitableNode<EnumValueDeclarator, DeclaratorBase> {
   public:
    EnumValueDeclarator(std::string identifier,
                        std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _identifier(std::move(identifier)), _expr(std::move(expr)){};

    std::string_view getIdentifier() const override {
        throw std::runtime_error(
            "getIdentifier() called on EnumValueDeclarator. This shouldn't happen.");
    };

    auto* getExpr() const {
        return _expr.get();
    }

   private:
    std::string _identifier;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
