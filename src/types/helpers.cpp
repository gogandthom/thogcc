#include "types/helpers.h"

#include <format>
#include <iterator>
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

std::string printType(const Type& type) {
    std::string res;
    if (type.isConst) res += "const ";
    if (type.isVolatile) res += "volatile ";

    res += std::visit(overload{
                          [type](const BasicType& b) -> std::string {
                              const std::string unsignedLabel = b.isUnsigned ? "unsigned " : "";
                              switch (b.kind) {
                                  case BasicType::Kind::VOID:
                                      return "void";
                                  case BasicType::Kind::CHAR:
                                      return "char";
                                  case BasicType::Kind::SHORT:
                                      return unsignedLabel + "short";
                                  case BasicType::Kind::INT:
                                      return unsignedLabel + "int";
                                  case BasicType::Kind::LONG:
                                      return unsignedLabel + "long";
                                  case BasicType::Kind::FLOAT:
                                      return "float";
                                  case BasicType::Kind::DOUBLE:
                                      return "double";
                              };
                          },
                          [type](const PointerType& p) { return printType(*p.pointsTo) + "*"; },
                          [type](const ArrayType& a) {
                              return std::format("{}[{}]", printType(*a.elementType), a.size);
                          },
                          [type](const FuncType& f) {
                              std::string paramLabel;
                              for (auto it = f.params.begin(); it != f.params.end(); ++it) {
                                  paramLabel += printType(**it);
                                  if (std::next(it) != f.params.end()) {
                                      paramLabel += ", ";
                                  }
                              }
                              return std::format("{}({})", printType(*f.returnType), paramLabel);
                          },
                          [type](const StructType&) -> std::string { return "struct"; },
                          [type](const UnionType&) -> std::string { return "union"; },
                          [type](const EnumType&) -> std::string { return "enum"; },
                      },
                      type.data);

    return res;
}

}  // namespace thogcc::types
