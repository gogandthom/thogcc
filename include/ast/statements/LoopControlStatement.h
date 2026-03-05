#pragma once

#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

class LoopControlStatement : public StatementBase {
   public:
    LoopControlStatement(bool isBreak = false) : _isBreak(isBreak){};

   private:
    bool _isBreak;
};

}  // namespace thogcc::ast::statements
