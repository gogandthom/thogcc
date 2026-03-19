#pragma once

#include <string>

#include "ir/LLVMType.h"
#include "types/Type.h"

namespace thogcc::types {

ir::LLVMType toLLVMType(const Type& type);

std::string printType(const Type& type);

}  // namespace thogcc::types
