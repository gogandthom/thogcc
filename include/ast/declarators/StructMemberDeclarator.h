#pragma once

#include <memory>
#include <utility>

#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class StructMemberDeclarator : public DeclaratorBase {
   public:
    StructMemberDeclarator(std::unique_ptr<DeclaratorBase> decl,
                           std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _decl(std::move(decl)), _expr(std::move(expr)){};

   private:
    std::unique_ptr<DeclaratorBase> _decl;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
