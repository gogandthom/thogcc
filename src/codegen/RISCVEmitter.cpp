#include "codegen/RISCVEmitter.h"

#include <cassert>
#include <format>
#include <string_view>
#include <variant>
#include <vector>

#include "ir/LLVMType.h"
#include "ir/llvm.h"

namespace thogcc::codegen {

void RISCVEmitter::loadValue(const ir::LLVMValueID& valID, std::string_view targetReg) {
    switch (valID.kind) {
        case ir::LLVMValueKind::PARAM:
        case ir::LLVMValueKind::INSTR:
            // TODO stack things
            break;
        case ir::LLVMValueKind::CONST:
            std::visit([this, targetReg](
                           auto& c) { _out << std::format("    li {}, {}\n", targetReg, c); },
                       _curFunc->consts.at(valID.id).value);
            break;
    }
}

void RISCVEmitter::emit(const ir::LLVMModule& module) {
    _out << std::format(".file \"{}\"\n", module.srcFileName);
    _out << ".option nopic\n";  // static binary, not a shared library
    // TODO .attribute arch, unaligned_access, stack_align

    _out << ".text\n";
    for (const auto& func : module.functions) {
        emitFunction(func);
    }
}

void RISCVEmitter::emitFunction(const ir::LLVMFunction& func) {
    // set context
    _curFunc = &func;

    _out << std::format(".globl {}\n", func.name);
    _out << std::format(".type {}, @function\n", func.name);
    _out << std::format("{}:\n", func.name);

    // prologue
    int frameSize = 32;
    _out << std::format("    addi sp, sp, -{}\n", frameSize);   // allocate stack
    _out << std::format("    sw ra, {}(sp)\n", frameSize - 4);  // save ra
    _out << std::format("    sw s0, {}(sp)\n", frameSize - 8);  // save old frame pointer
    _out << std::format("    addi s0, sp, {}\n", frameSize);    // set frame pointer

    for (const auto& block : func.blocks) {
        if (!block.label.empty()) _out << std::format("{}:\n", block.label);

        for (const auto& id : block.instrIDs) {
            emitInstruction(id);
        }
    }

    // epilogue
    _out << std::format(".L_{}_epilogue:\n", func.name);
    _out << std::format("    lw ra, {}(sp)\n", frameSize - 4);  // restore old frame pointer
    _out << std::format("    lw s0, {}(sp)\n", frameSize - 8);  // restore old frame pointer
    _out << std::format("    addi sp, sp, {}\n", frameSize);    // deallocate frame
    _out << "    jr ra\n";

    _out << std::format("    .size {0}, .-{0}\n", func.name);
}

void RISCVEmitter::emitInstruction(ir::LLVMInstrID instrID) {
    const ir::LLVMInstruction& instr = _curFunc->getInstr(instrID);

    switch (instr.opcode) {
        case ir::LLVMOpcode::ADD:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "    add t2, t0, t1\n";
            // sw

            break;
        case ir::LLVMOpcode::FADD: {
            std::string precision;
            if (instr.type.type == ir::LLVMBasicType::FLOAT) {
                precision = "s";
            } else if (instr.type.type == ir::LLVMBasicType::DOUBLE) {
                precision = "d";
            } else {
                assert(false && "Invalid type for FADD");
            };
            loadValue(instr.operands.at(0), "ft0");
            loadValue(instr.operands.at(1), "ft1");
            _out << std::format("    fadd.{} ft2, ft0, ft1\n", precision);
            // sw

            break;
        }
        case ir::LLVMOpcode::SUB:
        case ir::LLVMOpcode::FSUB:
        case ir::LLVMOpcode::MUL:
        case ir::LLVMOpcode::FMUL:
        case ir::LLVMOpcode::UDIV:
        case ir::LLVMOpcode::SDIV:
        case ir::LLVMOpcode::FDIV:
        case ir::LLVMOpcode::UREM:
        case ir::LLVMOpcode::SREM:
        case ir::LLVMOpcode::ICMP:
        case ir::LLVMOpcode::ALLOCA:
        case ir::LLVMOpcode::LOAD:
        case ir::LLVMOpcode::STORE:
        case ir::LLVMOpcode::BR:
        case ir::LLVMOpcode::CALL:
            break;
        case ir::LLVMOpcode::RET:
            // TODO load a0 retval
            _out << std::format("    j L_{}_epilogue\n", _curFunc->name);
            break;
        case ir::LLVMOpcode::FREM:
        case ir::LLVMOpcode::FCMP:
        case ir::LLVMOpcode::GETELEMENTPTR:
            break;
    }
}

}  // namespace thogcc::codegen
