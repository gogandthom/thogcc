#pragma once

namespace thogcc::ast {

enum class TypeSpecifier {
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
