#pragma once

#include <memory>
#include <utility>

#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class ArrayDeclarator : public DeclaratorBase {
   public:
    ArrayDeclarator(std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _expr(std::move(expr)){};
    ArrayDeclarator(std::unique_ptr<DeclaratorBase> base,
                    std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _base(std::move(base)), _expr(std::move(expr)){};

   private:
    std::unique_ptr<DeclaratorBase> _base;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
