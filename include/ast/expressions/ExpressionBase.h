#pragma once

#include "ast/Node.h"

namespace thogcc::ast::expressions {

class ExpressionBase : public VisitableNode<ExpressionBase> {};

}  // namespace thogcc::ast::expressions
