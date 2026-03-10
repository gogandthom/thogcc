#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include "ast/Node.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/Initializer.h"

namespace thogcc::ast::declarators {

class InitDeclarator : public VisitableNode<InitDeclarator, DeclaratorBase> {
   public:
    InitDeclarator(std::unique_ptr<DeclaratorBase> decl,
                   std::unique_ptr<expressions::Initializer> initizer = nullptr)
        : _decl(std::move(decl)), _initizer(std::move(initizer)){};

    std::string_view getIdentifier() const override {
        return _decl->getIdentifier();
    };

   private:
    std::unique_ptr<DeclaratorBase> _decl;
    std::unique_ptr<expressions::Initializer> _initizer;
};

}  // namespace thogcc::ast::declarators
