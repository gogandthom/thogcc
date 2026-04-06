#pragma once

#include <ostream>
#include <string_view>

#include "ir/LLVMModule.h"

namespace thogcc::codegen {

class RISCVEmitter {
   public:
    RISCVEmitter(std::ostream& out, const ir::LLVMModule& module) : _out(out), _module(&module) {
        emitModule(*_module);
    }

   private:
    void emitModule(const ir::LLVMModule& module);
    void emitFunction(const ir::LLVMFunction& func);
    void emitInstruction(ir::LLVMInstrID instrID);

    void loadValue(const ir::LLVMValueID& valID, std::string_view targetReg);

    void pushStack(int id, std::string_view srcReg);
    void loadFromStack(int id, std::string_view targetReg);

    std::ostream& _out;  // NOLINT
    const ir::LLVMModule* _module;

    const ir::LLVMFunction* _curFunc = nullptr;
};

}  // namespace thogcc::codegen
