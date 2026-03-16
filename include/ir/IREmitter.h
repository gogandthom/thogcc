#pragma once

#include <ostream>

#include "ir/llvm.h"

namespace thogcc::ir {

// Emits our very basic subset of LLVM
class IREmitter {
   public:
    IREmitter(std::ostream& out) : _out(out) {}

    void printFunction(const LLVMFunction& func);

   private:
    std::ostream& _out;
};

}  // namespace thogcc::ir
