#pragma once

#include <cstdint>
#include <format>
#include <string>

namespace thogcc::ir {

enum class LLVMBasicType : std::uint8_t {
    VOID,
    INT,
    FLOAT,
    DOUBLE,
    PTR,
};

struct LLVMType {
    LLVMBasicType type;
    int intSize;  // only used for int

    /// helper to get string representation of type
    std::string printType() const {
        switch (type) {
            case LLVMBasicType::INT:
                return std::format("i{}", intSize);
                break;
            case LLVMBasicType::FLOAT:
                return "float";
                break;
            case LLVMBasicType::DOUBLE:
                return "double";
                break;
            case LLVMBasicType::VOID:
                return "void";
            case LLVMBasicType::PTR:
                return "ptr";
                break;
        }
    }
};

}  // namespace thogcc::ir
