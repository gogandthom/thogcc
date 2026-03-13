#pragma once

#include "ast/Node.h"

namespace thogcc::ast::declarations {

class DeclarationBase : public VisitableNode<DeclarationBase> {};

}  // namespace thogcc::ast::declarations
