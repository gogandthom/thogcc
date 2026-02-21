#pragma once

#include <memory>

#include "ast/Node.h"
#include "ast/declarators/AbstractDeclarator.h"

namespace thogcc::ast {

template <std::derived_from<declarators::AbstractDeclarator> T>
class Pointer : public Node {
   public:
    Pointer(std::unique_ptr<T> ptr);

   private:
    std::unique_ptr<T> _ptr;
};

}  // namespace thogcc::ast
