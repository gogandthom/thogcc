#include "ir/helpers.h"
#include <format>
#include <stdexcept>
#include <string>
#include <variant>

#include "ir/LLVMModule.h"
#include "ir/LLVMType.h"

namespace thogcc::ir {

std::string getValueLabel(const LLVMFunction& func, const LLVMValueID& id) {
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
            break;
    }
    throw std::runtime_error("Invalid ValueID");
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
        default:
            throw std::runtime_error("Invalid ValueID for getTypeOf");
    }
}

}  // namespace thogcc::ir
