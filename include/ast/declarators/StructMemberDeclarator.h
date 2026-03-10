#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/ConstantExpression.h"

namespace thogcc::ast::declarators {

class StructMemberDeclarator : public VisitableNode<StructMemberDeclarator, DeclaratorBase> {
   public:
    StructMemberDeclarator(std::unique_ptr<DeclaratorBase> decl,
                           std::unique_ptr<expressions::ConstantExpression> expr = nullptr)
        : _decl(std::move(decl)), _expr(std::move(expr)){};

    std::string_view getIdentifier() const override {
        return _decl->getIdentifier();
    };

   private:
    std::unique_ptr<DeclaratorBase> _decl;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
