#pragma once

#include <cstdint>

namespace thogcc::ast {

#define TYPE_QUALIFIER \
    X(CONST)           \
    X(VOLATILE)

#define X(VAL) VAL,
enum class TypeQualifier : std::uint8_t { TYPE_QUALIFIER };
#undef X

}  // namespace thogcc::ast
