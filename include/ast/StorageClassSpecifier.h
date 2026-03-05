#pragma once

#include <cstdint>

namespace thogcc::ast {

enum class StorageClassSpecifier : std::uint8_t {
    TYPEDEF,
    EXTERN,
    STATIC,
    AUTO,
    REGISTER,
};

}
