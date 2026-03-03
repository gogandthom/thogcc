#pragma once

#include <memory>

#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/Initializer.h"

namespace thogcc::ast::declarators {

class InitDeclarator : public DeclaratorBase {
   public:
    InitDeclarator(std::unique_ptr<DeclaratorBase> decl,
                   std::unique_ptr<expressions::Initializer> initizer = nullptr)
        : _decl(std::move(decl)), _initizer(std::move(initizer)){};

   private:
    std::unique_ptr<DeclaratorBase> _decl;
    std::unique_ptr<expressions::Initializer> _initizer;
};

}  // namespace thogcc::ast::declarators
