#pragma once

#include <ostream>

#include "ir/llvm.h"

namespace thogcc::ir {

// Emits our very basic subset of LLVM
class IREmitter {
   public:
    IREmitter(std::ostream& out) : _out(out) {}

    void emitFunction(const LLVMFunction& func);

   private:
    void emitInstruction(const LLVMFunction& func, LLVMInstrID instrID);

    std::ostream& _out;  // NOLINT
};

}  // namespace thogcc::ir
