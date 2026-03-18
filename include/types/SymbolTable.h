#pragma once

#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "types/Type.h"

namespace thogcc::types {

struct VarSymbol {
    std::shared_ptr<Type> type;
};

struct FuncSymbol {
    std::shared_ptr<Type> type;
    bool isDefinition;
};

struct TypedefSymbol {
    std::shared_ptr<Type> type;
};

struct StructField {
    std::string name;
    std::shared_ptr<Type> type;
};

struct UnionField {
    std::string name;
    std::shared_ptr<Type> type;
};

struct StructSymbol {
    std::vector<StructField> members;
    bool isDefinition;
};

struct UnionSymbol {
    std::vector<UnionField> members;
    bool isDefinition;
};

struct EnumSymbol {
    std::map<std::string, int> variants;
    bool isDefinition;
};

typedef std::variant<VarSymbol, FuncSymbol, TypedefSymbol> OrdSymbol;
typedef std::variant<StructSymbol, UnionSymbol, EnumSymbol> TagSymbol;

struct Scope {
    std::map<std::string, TagSymbol> tagSymbols;
    std::map<std::string, OrdSymbol> ordinarySymbols;
};

class SymbolTable {
   public:
    SymbolTable() {
        pushScope();  // global scope
    };

    void pushScope() {
        _scopes.emplace_back();
    };
    void popScope() {
        assert(!_scopes.empty());
        _scopes.pop_back();
    };

    void addToScope(std::string identifier, BasicType type);

   private:
    std::vector<Scope> _scopes;
};

}  // namespace thogcc::types
