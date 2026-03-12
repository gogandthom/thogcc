#pragma once

#include <cstdint>

namespace thogcc::ast {

enum class TypeQualifier : std::uint8_t {
    CONST,
    VOLATILE,
};

}
