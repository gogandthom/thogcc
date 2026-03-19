#include "types/helpers.h"

#include <variant>

#include "errors/errors.h"
#include "ir/LLVMType.h"
#include "types/Type.h"
#include "utils.h"

namespace thogcc::types {

ir::LLVMType toLLVMType(const Type& type) {
    return std::visit(
        overload{
            [](const BasicType& t) -> ir::LLVMType {
                switch (t.kind) {
                    case BasicType::Kind::VOID:
                        return {ir::LLVMBasicType::VOID, 0};
                    case BasicType::Kind::CHAR:
                        return {ir::LLVMBasicType::INT, 8};
                    case BasicType::Kind::SHORT:
                        return {ir::LLVMBasicType::INT, 16};
                    case BasicType::Kind::INT:
                        return {ir::LLVMBasicType::INT, 32};
                    case BasicType::Kind::LONG:
                        return {ir::LLVMBasicType::INT, 64};
                    case BasicType::Kind::FLOAT:
                        return {ir::LLVMBasicType::FLOAT, 0};
                    case BasicType::Kind::DOUBLE:
                        return {ir::LLVMBasicType::DOUBLE, 0};
                    default:
                        throw errors::SemaError("Error converting C Type to LLVMType");
                };
            },
            [](const PointerType&) -> ir::LLVMType { return {ir::LLVMBasicType::PTR, 0}; },
            [](const ArrayType&) -> ir::LLVMType { return {ir::LLVMBasicType::PTR, 0}; },
            [](const FuncType&) -> ir::LLVMType { return {ir::LLVMBasicType::PTR, 0}; },
            [](const StructType&) -> ir::LLVMType {
                throw errors::SemaError("Unimplemented type conversion: StructType to LLVMType");
            },
            [](const UnionType&) -> ir::LLVMType {
                throw errors::SemaError("Unimplemented type conversion: UnionType to LLVMType");
            },
            [](const EnumType&) -> ir::LLVMType { return {ir::LLVMBasicType::INT, 32}; },
        },
        type.data);
}

}  // namespace thogcc::types
