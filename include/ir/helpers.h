#pragma once

#include <string>

#include "LLVMModule.h"
#include "ir/LLVMType.h"

namespace thogcc::ir {

std::string getValueLabel(const LLVMModule& mod, const LLVMFunction& func, const LLVMValueID& id);

const LLVMInstruction& getInstr(const LLVMFunction& func, LLVMInstrID id);

LLVMType getTypeOf(const LLVMFunction& func, const LLVMValueID& id);

std::string printType(LLVMType type);

}  // namespace thogcc::ir
