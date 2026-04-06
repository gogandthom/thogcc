#pragma once

#include "ast/Node.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class LoopControlStatement : public VisitableNode<LoopControlStatement, StatementBase> {
   public:
    LoopControlStatement(bool isBreak = false) : _isBreak(isBreak) {}

    bool getIsBreak() const {
        return _isBreak;
    }

   private:
    bool _isBreak;
};

}  // namespace thogcc::ast::statements
