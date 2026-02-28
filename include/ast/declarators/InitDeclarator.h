#pragma once

#include <memory>

#include "ast/declarators/DeclaratorBase.h"
#include "ast/expressions/Initializer.h"

namespace thogcc::ast::declarators {

class InitDeclarator : public DeclaratorBase {
   public:
    InitDeclarator(std::unique_ptr<DeclaratorBase> decl,
                   std::unique_ptr<expressions::Initializer> initizer = nullptr);

   private:
    std::unique_ptr<DeclaratorBase> decl;
    std::unique_ptr<expressions::Initializer> initizer;
};

}  // namespace thogcc::ast::declarators
