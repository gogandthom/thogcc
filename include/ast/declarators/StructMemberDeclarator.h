#pragma once

#include <memory>

#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class StructMemberDeclarator : public DeclaratorBase {
   public:
    StructMemberDeclarator(std::unique_ptr<DeclaratorBase> declarator,
                           std::unique_ptr<expressions::ConstantExpression> expr = nullptr);

   private:
    std::unique_ptr<DeclaratorBase> declarator;
    std::unique_ptr<expressions::ConstantExpression> expr;
};

}  // namespace thogcc::ast::declarators
