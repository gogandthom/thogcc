#pragma once

#include "ast/Node.h"

namespace thogcc::ast::statements {

class StatementBase : public VisitableNode<StatementBase> {};

}  // namespace thogcc::ast::statements
