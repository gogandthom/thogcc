#pragma once

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace thogcc {

enum class Type {
    VOID,
    i8,   // char
    i16,  // short
    i32,  // int
    i64,  // long
    u8,
    u16,
    u32,
    u64,
    FLOAT,   // 32 bit
    DOUBLE,  // 64 bit
    ptr,     // weird
};

class SymbolTable {
   public:
    SymbolTable();

    typedef struct Symbol {
        Type returnType;
        struct qualifiers {
            bool isConst : 1;
            bool isVolatile : 1;
        } qualifiers;
        std::shared_ptr<Symbol> pointsTo;
        std::vector<Type> arguments;
    } Symbol;

    typedef struct {
        std::map<std::string, Symbol> symbols;
    } StackFrame;

    void pushScope();
    StackFrame popScope();

    void addToScope(std::string identifier, Type type);

   private:
    std::stack<StackFrame> stack;
};

}  // namespace thogcc
