#pragma once

#include <string_view>

#include "ast/Node.h"

namespace thogcc::ast::declarators {

class DeclaratorBase : public VisitableNode<DeclaratorBase> {
   public:
    virtual std ::string_view getIdentifier() const = 0;
};

}  // namespace thogcc::ast::declarators
