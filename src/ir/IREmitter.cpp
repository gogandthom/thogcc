#include "ir/IREmitter.h"

#include <format>
#include <iterator>
#include <stdexcept>
#include <string>
#include <variant>

#include "ir/LLVMModule.h"
#include "ir/LLVMType.h"
#include "ir/helpers.h"

namespace thogcc::ir {

void IREmitter::emitModule(const LLVMModule& module) {
    _out << std::format("source_filename = \"{}\"\n", module.srcFileName);

    if (!module.globals.empty()) {
        _out << "\n";
        for (const auto& global : module.globals) {
            _out << std::format(
                "@{} = global {} {}\n", global.name, printType(global.type),
                std::visit([](auto& val) { return std::to_string(val); }, global.initValue));
        }
    }

    _out << "\n";
    for (const auto& func : module.functions) {
        emitFunction(func);
    }
}

void IREmitter::emitInstruction(const LLVMFunction& func, LLVMInstrID instrID) {
    const LLVMInstruction& instr = getInstr(func, instrID);

    _out << "  ";  // indent

    // Destination register
    const bool hasDest = (instr.type.type != LLVMBasicType::VOID) &&
                         (instr.opcode != LLVMOpcode::RET);  // TODO is this correct?
    if (hasDest) {
        _out << std::format("%ins{} = ", instrID.id);
    }

    switch (instr.opcode) {
        // Binary operations
        case LLVMOpcode::ADD:
        case LLVMOpcode::FADD:
        case LLVMOpcode::SUB:
        case LLVMOpcode::FSUB:
        case LLVMOpcode::MUL:
        case LLVMOpcode::FMUL:
        case LLVMOpcode::UDIV:
        case LLVMOpcode::SDIV:
        case LLVMOpcode::FDIV:
        case LLVMOpcode::UREM:
        case LLVMOpcode::SREM: {
            // opcode and type
            _out << std::format("{} {}", printOpcode(instr.opcode), printType(instr.type));
            // operands
            _out << std::format(" {}, {}", getValueLabel(*_module, func, instr.operands.at(0)),
                                getValueLabel(*_module, func, instr.operands.at(1)));
            break;
        }

        // CMP
        case LLVMOpcode::ICMP: {
            // opcode and cond
            const LLVMType opType = getTypeOf(func, instr.operands.at(0));
            _out << std::format("icmp {} {}", printCmpCond(instr.cond), printType(opType));
            // operands
            _out << std::format(" {}, {}", getValueLabel(*_module, func, instr.operands.at(0)),
                                getValueLabel(*_module, func, instr.operands.at(1)));
            break;
        }
        case LLVMOpcode::FCMP:
            break;

        // Memory
        case LLVMOpcode::ALLOCA:
            // opcode and type
            _out << std::format("{} {}", printOpcode(instr.opcode), printType(instr.type));
            break;
        case LLVMOpcode::LOAD:
            _out << std::format("{} {}", printOpcode(instr.opcode), printType(instr.type));
            _out << std::format(", ptr {}", getValueLabel(*_module, func, instr.operands.at(0)));
            break;
        case LLVMOpcode::STORE:
            _out << std::format("{} {} {}, ptr {}", printOpcode(instr.opcode),
                                printType(getTypeOf(func, instr.operands.at(0))),
                                getValueLabel(*_module, func, instr.operands.at(0)),
                                getValueLabel(*_module, func, instr.operands.at(1)));
            break;
        case LLVMOpcode::GETELEMENTPTR:
            break;

        // Control flow
        case LLVMOpcode::BR:
            if (instr.operands.size() == 1) {
                _out << std::format("br label {}",
                                    getValueLabel(*_module, func, instr.operands.at(0)));
            } else {
                _out << std::format("br i1 {}, label {}, label {}",
                                    getValueLabel(*_module, func, instr.operands.at(0)),
                                    getValueLabel(*_module, func, instr.operands.at(1)),
                                    getValueLabel(*_module, func, instr.operands.at(2)));
            }
            break;
        case LLVMOpcode::CALL:
            break;
        case LLVMOpcode::RET:
            // opcode and type
            _out << std::format("{} {}", printOpcode(instr.opcode), printType(instr.type));
            if (instr.type.type != LLVMBasicType::VOID) {
                _out << " " << getValueLabel(*_module, func, instr.operands.at(0));
            }
            break;
    }

    _out << "\n";
}

void IREmitter::emitFunction(const LLVMFunction& func) {
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
            // TODO this should go in IRChecker
            throw std::runtime_error(
                std::format("Unlabelled non-entry block in function {}", func.name));
        }
        first = false;

        // Labels for subsequent blocks
        if (!block.label.empty()) {
            _out << block.label << ":\n";
        }

        for (const auto& id : block.instrIDs) {
            emitInstruction(func, id);
        }
    }

    _out << "}\n";
}

}  // namespace thogcc::ir
