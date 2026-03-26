#pragma once

#include <cstdint>

namespace thogcc::ir {

enum class LLVMBasicType : std::uint8_t {
    VOID,
    INT,
    FLOAT,
    DOUBLE,
    PTR,
};

struct LLVMType {
    LLVMBasicType type = LLVMBasicType::VOID;
    int intSize{};  // only used for int
};

}  // namespace thogcc::ir
