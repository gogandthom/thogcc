#pragma once

#include <memory>

#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

class PointerDeclarator : public DeclaratorBase {
   public:
    PointerDeclarator(std::unique_ptr<DeclaratorBase> ptr = nullptr) : _ptr(std::move(ptr)){};
    /// Attach node to leaf
    void attach(std::unique_ptr<DeclaratorBase> node);

   private:
    std::unique_ptr<DeclaratorBase> _ptr;
};

}  // namespace thogcc::ast::declarators
