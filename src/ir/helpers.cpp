#include "ir/helpers.h"

#include <cassert>
#include <format>
#include <stdexcept>
#include <string>
#include <variant>

#include "ir/LLVMModule.h"
#include "ir/LLVMType.h"

namespace thogcc::ir {

std::string getValueLabel(const LLVMModule& mod, const LLVMFunction& func, const LLVMValueID& id) {
    switch (id.kind) {
        case LLVMValueKind::INSTR:
            return std::format("%ins{}", id.id);
        case LLVMValueKind::PARAM:
            return std::format("%{}", func.params.at(id.id).name);
        case LLVMValueKind::CONST:
            return std::visit([](const auto& c) { return std::to_string(c); },
                              func.consts.at(id.id).value);
        case LLVMValueKind::BLOCK:
            return std::format("{}", func.blocks.at(id.id).label);
        case LLVMValueKind::GLOBAL:
            return std::format("@{}", mod.globals.at(id.id).name);
    }
    assert(false && "Unimplemented LLVMValueKind for getValueLabel");
    __builtin_unreachable();
}

const LLVMInstruction& getInstr(const LLVMFunction& func, LLVMInstrID id) {
    return func.instructions.at(id.id);
}

LLVMType getTypeOf(const LLVMFunction& func, const LLVMValueID& id) {
    switch (id.kind) {
        case LLVMValueKind::PARAM:
            return func.params.at(id.id).type;
        case LLVMValueKind::INSTR:
            return func.instructions.at(id.id).type;
        case LLVMValueKind::CONST:
            return func.consts.at(id.id).type;
        case LLVMValueKind::BLOCK:
            assert(false &&
                   "getTypeOf() called on LLVMValueKind::BLOCK, should this be happening?");
            return {LLVMBasicType::PTR};
        case LLVMValueKind::GLOBAL:
            throw std::runtime_error("Unimplemented: getTypeOf() called with global");
    }
    assert(false && "Invalid ValueID for getTypeOf");
    __builtin_unreachable();
}

std::string printType(LLVMType type) {
    switch (type.type) {
        case LLVMBasicType::INT:
            return std::format("i{}", type.intSize);
        case LLVMBasicType::FLOAT:
            return "float";
        case LLVMBasicType::DOUBLE:
            return "double";
        case LLVMBasicType::VOID:
            return "void";
        case LLVMBasicType::PTR:
            return "ptr";
    }
    assert(false && "Invalid LLVMBasicType for printType");
    __builtin_unreachable();
}

}  // namespace thogcc::ir
