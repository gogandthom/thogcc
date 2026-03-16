#include "ir/IREmitter.h"

#include <format>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "ir/llvm.h"

namespace thogcc::ir {

// helper to get string representation of type
constexpr std::string printType(const LLVMType& type) {
    switch (type.type) {
        case LLVMBasicType::INT:
            return std::format("i{}", type.intSize);
            break;
        case LLVMBasicType::FLOAT:
            return "float";
            break;
        case LLVMBasicType::DOUBLE:
            return "double";
            break;
        case LLVMBasicType::VOID:
            return "void";
        case LLVMBasicType::PTR:
            return "ptr";  // TODO is this correct?
            break;
    }
}

// helper to get opcode
constexpr std::string printOpcode(const LLVMOpcode& op) {
    switch (op) {
#define X(VAL, NAME)      \
    case LLVMOpcode::VAL: \
        return NAME;
        LLVM_OPCODE
#undef X
    }
};

// helper to get a single instruction
std::string printInstruction(const LLVMFunction& func, LLVMInstrID instrID) {
    const LLVMInstruction& instr = func.getInstr(instrID);

    std::stringstream res;
    res << "  ";  // indent

    // Destination register
    bool hasDest = (instr.type.type != LLVMBasicType::VOID) &&
                   (instr.opcode != LLVMOpcode::RET);  // TODO is this correct?
    if (hasDest) {
        res << "%" << instrID.id << " = ";
    }

    // opcode and type
    res << printOpcode(instr.opcode) << " " << printType(instr.type);

    // operands
    switch (instr.opcode) {
        case LLVMOpcode::ADD:
        case LLVMOpcode::FADD:
            res << " " << func.getValueLabel(instr.operands.at(0)) << ", "
                << func.getValueLabel(instr.operands.at(0));
            break;
        case LLVMOpcode::SUB:
        case LLVMOpcode::MUL:
        case LLVMOpcode::ALLOCA:
        case LLVMOpcode::FSUB:
        case LLVMOpcode::FMUL:
        case LLVMOpcode::UDIV:
        case LLVMOpcode::SDIV:
        case LLVMOpcode::FDIV:
        case LLVMOpcode::UREM:
        case LLVMOpcode::SREM:
        case LLVMOpcode::ICMP:
        case LLVMOpcode::LOAD:
        case LLVMOpcode::STORE:
        case LLVMOpcode::BR:
        case LLVMOpcode::CALL:
            break;
        case LLVMOpcode::RET:
            res << " " << func.getValueLabel(instr.operands.at(0));
            break;
    }

    return res.str();
}

void IREmitter::printFunction(const LLVMFunction& func) {
    // Name and return type
    _out << "define " << printType(func.returnType) << " @" << func.name << "(";

    // Parameters
    for (auto it = func.params.begin(); it != func.params.end(); ++it) {
        _out << printType(it->type) << " %" << it->name;
        if (std::next(it) != func.params.end()) {
            _out << ", ";
        }
    }

    _out << ") {\n";

    // Blocks
    bool first = true;
    for (const auto& block : func.blocks) {
        // First block can be unnamed
        if (block.label.empty() && !first) {
            throw std::runtime_error(
                std::format("Unlabelled non-entry block in function {}", func.name));
        }
        first = false;

        // Labels for subsequent blocks
        if (!block.label.empty()) {
            _out << block.label << ":\n";
        }

        for (const auto& id : block.instrIDs) {
            _out << printInstruction(func, id) << "\n";
        }
    }

    _out << "}\n";
}

}  // namespace thogcc::ir
