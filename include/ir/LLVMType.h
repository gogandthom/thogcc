#pragma once

namespace thogcc::ir {

enum class LLVMBasicType { VOID, INT, FLOAT, DOUBLE, PTR };

struct LLVMType {
    LLVMBasicType type;
    int intSize;  // only used for int
};

}  // namespace thogcc::ir
