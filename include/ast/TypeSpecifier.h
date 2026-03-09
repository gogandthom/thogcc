#pragma once

#include <cstdint>

namespace thogcc::ast {

#define TYPE_SPECIFIER \
    X(VOID)            \
    X(CHAR)            \
    X(SHORT)           \
    X(INT)             \
    X(LONG)            \
    X(FLOAT)           \
    X(DOUBLE)          \
    X(SIGNED)          \
    X(UNSIGNED)        \
    X(STRUCT)          \
    X(ENUM)            \
    X(TYPE_NAME)

enum class TypeSpecifier : std::uint8_t {
#define X(VAL) VAL,
    TYPE_SPECIFIER
#undef X
};

}  // namespace thogcc::ast
