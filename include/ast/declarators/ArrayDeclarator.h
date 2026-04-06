#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class ArrayDeclarator : public VisitableNode<ArrayDeclarator, DeclaratorBase> {
   public:
    ArrayDeclarator(std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _expr(std::move(expr)) {}
    ArrayDeclarator(std::unique_ptr<DeclaratorBase> base,
                    std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _base(std::move(base)), _expr(std::move(expr)) {}

    std::string_view getIdentifier() const override {
        if (!_base) {
            throw std::runtime_error("getIdentifier() called on an abstract ArrayDeclarator.");
        }
        return _base->getIdentifier();
    }

    auto* getBase() const {
        return _base.get();
    }
    auto* getExpr() const {
        return _expr.get();
    }

   private:
    std::unique_ptr<DeclaratorBase> _base;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
