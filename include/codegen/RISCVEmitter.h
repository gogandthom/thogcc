#pragma once

#include <map>
#include <ostream>
#include <string_view>

#include "ir/LLVMModule.h"

namespace thogcc::codegen {

class RISCVEmitter {
   public:
    RISCVEmitter(std::ostream& out) : _out(out) {}

    void emit(const ir::LLVMModule& module);

   private:
    void emitFunction(const ir::LLVMFunction& func);
    void emitInstruction(ir::LLVMInstrID instrID);

    void loadValue(const ir::LLVMValueID& valID, std::string_view targetReg);

    void pushStack(int id, std::string_view srcReg);
    void loadFromStack(int id, std::string_view targetReg);

    std::ostream& _out;  // NOLINT

    const ir::LLVMFunction* _curFunc = nullptr;
};

}  // namespace thogcc::codegen
