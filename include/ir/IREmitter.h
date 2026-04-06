#pragma once

#include <ostream>

#include "ir/LLVMModule.h"

namespace thogcc::ir {

// Emits our very basic subset of LLVM
class IREmitter {
   public:
    IREmitter(std::ostream& out, const LLVMModule& module) : _out(out), _module(&module) {
        emitModule(*_module);
    }

   private:
    void emitModule(const LLVMModule& module);
    void emitFunction(const LLVMFunction& func);
    void emitInstruction(const LLVMFunction& func, LLVMInstrID instrID);

    std::ostream& _out;  // NOLINT
    const LLVMModule* _module;
};

}  // namespace thogcc::ir
