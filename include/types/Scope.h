#pragma once

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

typedef std::variant<std::shared_ptr<VarSymbol>, std::shared_ptr<FuncSymbol>,
                     std::shared_ptr<TypedefSymbol>>
    OrdSymbol;
typedef std::variant<std::shared_ptr<StructSymbol>, std::shared_ptr<UnionSymbol>,
                     std::shared_ptr<EnumSymbol>>
    TagSymbol;

struct Scope {
    std::map<std::string, TagSymbol> tagSymbols;
    std::map<std::string, OrdSymbol> ordinarySymbols;
};

}  // namespace thogcc::types
