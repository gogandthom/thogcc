#pragma once

#include <memory>

#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class ArrayDeclarator : public DeclaratorBase {
   public:
    ArrayDeclarator(std::unique_ptr<expressions::ConstantExpression> expr = nullptr);
    ArrayDeclarator(std::unique_ptr<DeclaratorBase> base,
                    std::unique_ptr<expressions::ConstantExpression> expr = nullptr);

   private:
    std::unique_ptr<DeclaratorBase> _base;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
