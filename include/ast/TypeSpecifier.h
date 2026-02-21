#pragma once

#include <cstdint>

namespace thogcc::ast {

enum class TypeSpecifier : std::uint8_t {
    VOID,
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    SIGNED,
    UNSIGNED,
    STRUCT,
    ENUM,
    TYPE_NAME
};

}
