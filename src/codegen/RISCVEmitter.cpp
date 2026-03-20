#include "codegen/RISCVEmitter.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "ir/LLVMType.h"
#include "ir/llvm.h"
#include "utils.h"

namespace thogcc::codegen {

const int prologueSize = 8;

const int stackItemSize = 8;

void RISCVEmitter::loadValue(const ir::LLVMValueID& valID, std::string_view targetReg) {
    switch (valID.kind) {
        case ir::LLVMValueKind::PARAM:
            if (valID.id < 8) {
                _out << std::format("    mv {}, a{}\n", targetReg, valID.id);
            } else {
                throw std::runtime_error("Unimplemented: params > 7");
            }
            break;
        case ir::LLVMValueKind::INSTR:
            if (this->_allocaInsts.contains(valID.id)) {
                _out << std::format("    addi {}, s0, -{}\n", targetReg,
                                    this->_allocaInsts[valID.id]);
            } else {
                loadFromStack(valID.id, targetReg);
            }
            break;
        case ir::LLVMValueKind::CONST:
            std::visit(
                overload{
                    [this, targetReg](const uint64_t& c) {
                        _out << std::format("    li {}, {}\n", targetReg, c);
                    },
                    [this, targetReg, valID](const float&) {
                        std::string label = std::format(".LC_{}_{}", _curFunc->name, valID.id);
                        _out << std::format("    lla t0, {}\n", label);
                        _out << std::format("    flw {}, 0(t0)\n", targetReg);
                    },
                    [this, targetReg, valID](const double&) {
                        std::string label = std::format(".LC_{}_{}", _curFunc->name, valID.id);
                        _out << std::format("    lla t0, {}\n", label);
                        _out << std::format("    flw {}, 0(t0)\n", targetReg);
                    },
                },
                _curFunc->consts.at(valID.id).value);
            break;
    }
}

void RISCVEmitter::pushStack(int id, std::string_view srcReg) {
    // _out << std::format("    addi sp, sp, -{}\n", stackItemSize);
    _out << std::format("    sw {}, -{}(s0)\n", srcReg, prologueSize + (id * stackItemSize));
}

void RISCVEmitter::loadFromStack(int id, std::string_view targetReg) {
    _out << std::format("    lw {}, -{}(s0)\n", targetReg, prologueSize + (id * stackItemSize));
}

void RISCVEmitter::clearStack() {
    _out << std::format("    addi sp, s0, -{}\n", prologueSize);
}

void RISCVEmitter::emit(const ir::LLVMModule& module) {
    _out << std::format(".file \"{}\"\n", module.srcFileName);
    _out << ".option nopic\n";  // static binary, not a shared library
    // TODO .attribute arch, unaligned_access, stack_align

    // Float consts
    if (!module.functions.empty()) {
        _out << ".section .rodata\n";
        for (const auto& func : module.functions) {
            for (size_t i = 0; i < func.consts.size(); ++i) {
                auto c = func.consts[i];
                if (std::holds_alternative<float>(c.value)) {
                    _out << std::format(".LC_{}_{}:\n", func.name, i);
                    _out << std::format("  .float {}\n", std::get<float>(c.value));
                } else if (std::holds_alternative<double>(c.value)) {
                    _out << std::format(".LC_{}_{}:\n", func.name, i);
                    _out << std::format("  .double {}\n", std::get<double>(c.value));
                }
            }
        }
    }

    // Globals
    if (!module.globals.empty()) {
        _out << ".data\n";
        for (const auto& global : module.globals) {
            _out << std::format(".type {}, @object\n", global.name);
            _out << std::format(".globl {}\n", global.name);
            _out << std::format("{}:\n", global.name);
            std::visit(overload{
                           [this, &global](const uint64_t& val) {
                               _out << std::format("    .word {}\n", val);
                               _out << std::format("    .size {}, 4\n", global.name);
                           },
                           [this, &global](const double& val) {
                               _out << std::format("    .double {:f}\n", val);
                               _out << std::format("    .size {}, 8\n", global.name);
                           },
                       },
                       global.initValue);
        }
    }

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
    int frameSize = prologueSize;
    _out << std::format("    addi sp, sp, -{}\n", frameSize);   // allocate stack
    _out << std::format("    sw ra, {}(sp)\n", frameSize - 4);  // save ra
    _out << std::format("    sw s0, {}(sp)\n", frameSize - 8);  // save old frame pointer
    _out << std::format("    addi s0, sp, {}\n", frameSize);    // set frame pointer

    for (const auto& block : func.blocks) {
        if (!block.label.empty()) {
            _out << std::format(".L_{}_{}:\n", func.name, block.label);
            _out << "    nop\n";
        }

        for (const auto& id : block.instrIDs) {
            emitInstruction(id);
        }
    }

    // epilogue
    _out << std::format(".L_{}_epilogue:\n", func.name);
    clearStack();
    _out << std::format("    lw ra, {}(sp)\n", frameSize - 4);  // restore old frame pointer
    _out << std::format("    lw s0, {}(sp)\n", frameSize - 8);  // restore old frame pointer
    _out << std::format("    addi sp, sp, {}\n", frameSize);    // deallocate frame
    _out << "    ret\n";

    _out << std::format("    .size {0}, .-{0}\n", func.name);

    this->_allocaInsts = {};
}

void RISCVEmitter::emitInstruction(ir::LLVMInstrID instrID) {
    const ir::LLVMInstruction& instr = _curFunc->getInstr(instrID);

    switch (instr.opcode) {
        case ir::LLVMOpcode::ADD:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "    add t2, t0, t1\n";
            pushStack(instrID.id, "t2");
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
            pushStack(instrID.id, "ft2");
            break;
        }
        case ir::LLVMOpcode::SUB:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "   sub t2, t0, t1\n";
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::FSUB: {
            std::string precision;
            if (instr.type.type == ir::LLVMBasicType::FLOAT) {
                precision = "s";
            } else if (instr.type.type == ir::LLVMBasicType::DOUBLE) {
                precision = "d";
            } else {
                assert(false && "Invalid type for FSUB");
            };
            loadValue(instr.operands.at(0), "ft0");
            loadValue(instr.operands.at(1), "ft1");
            _out << std::format("    fsub.{} ft2, ft0, ft1\n", precision);
            pushStack(instrID.id, "ft2");
            break;
        }
        case ir::LLVMOpcode::MUL:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "   mul t2, t0, t1\n";
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::FMUL: {
            std::string precision;
            if (instr.type.type == ir::LLVMBasicType::FLOAT) {
                precision = "s";
            } else if (instr.type.type == ir::LLVMBasicType::DOUBLE) {
                precision = "d";
            } else {
                assert(false && "Invalid type for FMUL");
            };
            loadValue(instr.operands.at(0), "ft0");
            loadValue(instr.operands.at(1), "ft1");
            _out << std::format("    fmul.{} ft2, ft0, ft1\n", precision);
            pushStack(instrID.id, "ft2");
            break;
        }
        case ir::LLVMOpcode::UDIV:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "   divu t2, t0, t1\n";
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::SDIV:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "   div t2, t0, t1\n";
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::FDIV: {
            std::string precision;
            if (instr.type.type == ir::LLVMBasicType::FLOAT) {
                precision = "s";
            } else if (instr.type.type == ir::LLVMBasicType::DOUBLE) {
                precision = "d";
            } else {
                assert(false && "Invalid type for FDIV");
            };
            loadValue(instr.operands.at(0), "ft0");
            loadValue(instr.operands.at(1), "ft1");
            _out << std::format("    fdiv.{} ft2, ft0, ft1\n", precision);
            pushStack(instrID.id, "ft2");
            break;
        }
        case ir::LLVMOpcode::UREM:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "   remu t2, t0, t1\n";
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::SREM:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "   rem t2, t0, t1\n";
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::ICMP:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            switch (instr.cond) {
                case ir::LLVMCmpCond::EQ:
                    _out << "    sub t2, t0, t1\n";
                    _out << "    seqz t2, t2\n";
                    break;
                case ir::LLVMCmpCond::NE:
                    _out << "    sub t2, t0, t1\n";
                    _out << "    snez t2, t2\n";
                    break;
                case ir::LLVMCmpCond::UGT:
                    _out << "    sltu t2, t1, t0\n";
                    break;
                case ir::LLVMCmpCond::UGE:
                    _out << "    sltu t2, t0, t1\n";
                    _out << "    xori t2, t2, 1\n";
                    break;
                case ir::LLVMCmpCond::ULT:
                    _out << "    sltu t2, t0, t1\n";
                    break;
                case ir::LLVMCmpCond::SGT:
                    _out << "    slt t2, t1, t0\n";
                    break;
                case ir::LLVMCmpCond::SGE:
                    _out << "    slt t2, t0, t1\n";
                    _out << "    xori t2, t2, 1\n";
                    break;
                case ir::LLVMCmpCond::SLT:
                    _out << "    slt t2, t0, t1\n";
                    break;
                case ir::LLVMCmpCond::SLE:
                    _out << "    slt t2, t1, t0\n";
                    _out << "    xori t2, t2, 1\n";  // invert
                    break;
                default:
                    assert(false && "Unimplemented ICMP instructionin RV backend");
            }
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::ALLOCA:
            this->_allocaInsts[instrID.id] = prologueSize + (instrID.id * stackItemSize);
            // _out << "    add t0, sp, zero\n";
            pushStack(instrID.id, "zero");
            break;
        case ir::LLVMOpcode::LOAD:
            loadValue(instr.operands.at(0), "t0");
            _out << "    lw t1, 0(t0)\n";
            pushStack(instrID.id, "t1");
            break;
        case ir::LLVMOpcode::STORE:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << "    sw t0, 0(t1)\n";
            pushStack(instrID.id, "zero");
            break;
        case ir::LLVMOpcode::BR:
            // unconditional jump
            pushStack(instrID.id, "zero");
            if (instr.operands.size() == 1) {
                _out << std::format("    j .L_{}_{}\n", _curFunc->name,
                                    _curFunc->getValueLabel(instr.operands.at(0)));
            } else {
                loadValue(instr.operands.at(0), "t0");
                _out << std::format("    bnez t0, .L_{}_{}\n", _curFunc->name,
                                    _curFunc->getValueLabel(instr.operands.at(1)));
                _out << std::format("    j .L_{}_{}\n", _curFunc->name,
                                    _curFunc->getValueLabel(instr.operands.at(2)));
            }
            break;
        case ir::LLVMOpcode::CALL:
            break;
        case ir::LLVMOpcode::RET:
            loadValue(instr.operands.at(0), "a0");
            pushStack(instrID.id, "zero");
            _out << std::format("    j .L_{}_epilogue\n", _curFunc->name);
            break;

        case ir::LLVMOpcode::FCMP:
        case ir::LLVMOpcode::GETELEMENTPTR:
            break;
    }
}

}  // namespace thogcc::codegen
