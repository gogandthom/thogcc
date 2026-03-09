#pragma once

#include <memory>
#include <string>
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

   private:
    std::string _identifier;
    std::unique_ptr<expressions::ConstantExpression> _expr;
};

}  // namespace thogcc::ast::declarators
