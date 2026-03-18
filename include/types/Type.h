#pragma once

#include <memory>
#include <variant>
#include <vector>

namespace thogcc::types {

struct BasicType {
    enum {
        VOID,
        CHAR,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
    } kind;
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

struct Type {
    std::variant<BasicType, PointerType, ArrayType, FuncType> data;
    bool isConst, isVolatile;  // type_qualifier can apply to any level
};

}  // namespace thogcc::types
