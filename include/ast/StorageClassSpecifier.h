#pragma once

#include <cstdint>

namespace thogcc::ast {

#define STORAGE_CLASS_SPECIFIER \
    X(TYPEDEF)                  \
    X(EXTERN)                   \
    X(STATIC)                   \
    X(AUTO)                     \
    X(REGISTER)

#define X(VAL) VAL,
enum class StorageClassSpecifier : std::uint8_t { STORAGE_CLASS_SPECIFIER };
#undef X

}  // namespace thogcc::ast
