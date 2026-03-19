#pragma once

#include "ir/LLVMType.h"
#include "types/Type.h"

namespace thogcc::types {

ir::LLVMType toLLVMType(const Type& type);

}
