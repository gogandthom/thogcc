#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <vector>

namespace thogcc::types {

struct BasicType {
    enum class Kind : std::uint8_t {
        VOID,
        CHAR,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
    } kind = BasicType::Kind::VOID;
    bool isUnsigned = false;
};

struct PointerType {
    std::shared_ptr<struct Type> pointsTo;
};

struct ArrayType {
    std::shared_ptr<struct Type> elementType;
    int size;
};

struct FuncType {
    std::shared_ptr<struct Type> returnType;
    std::vector<std::shared_ptr<struct Type>> params;
};

struct StructType {
    std::shared_ptr<struct StructSymbol> symbol;
};

struct UnionType {
    std::shared_ptr<struct UnionSymbol> symbol;
};

struct EnumType {
    std::shared_ptr<struct EnumSymbol> symbol;
};

struct Type {
    std::variant<BasicType, PointerType, ArrayType, FuncType, StructType, UnionType, EnumType> data;
    bool isConst = false, isVolatile = false;  // type_qualifier can apply to any level
};

}  // namespace thogcc::types
