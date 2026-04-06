#include "codegen/RISCVEmitter.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "ir/LLVMModule.h"
#include "ir/LLVMType.h"
#include "ir/helpers.h"
#include "utils.h"

// NOLINTBEGIN(*-magic-numbers)

namespace thogcc::codegen {

const unsigned int prologueSize = 8;

const unsigned int stackItemSize = 8;

static constexpr int getSlotOffset(std::size_t id) {
    return static_cast<int>(prologueSize + (id * stackItemSize));
}

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
            // alloca instructions don't do anything at runtime
            // we simply return the address of its stack slot
            if (ir::getInstr(*_curFunc, {valID.id}).opcode == ir::LLVMOpcode::ALLOCA) {
                _out << std::format("    addi {}, s0, -{}\n", targetReg, getSlotOffset(valID.id));
            }
            // and for regular instructions (temporary registers),
            // we load the value from the stack
            else {
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
                        _out << std::format("    la {}, {}\n", targetReg, label);
                        _out << std::format("    flw {0}, 0({0})\n", targetReg);
                    },
                    [this, targetReg, valID](const double&) {
                        std::string label = std::format(".LC_{}_{}", _curFunc->name, valID.id);
                        _out << std::format("    la {}, {}\n", targetReg, label);
                        _out << std::format("    fld {0}, 0({0})\n", targetReg);
                    },
                },
                _curFunc->consts.at(valID.id).value);
            break;
        case ir::LLVMValueKind::BLOCK:
            assert(false && "loadValue() passed a label (LLVMValueKind::BLOCK)");
            break;
        case ir::LLVMValueKind::GLOBAL:
            // Globals are always ptr, so we load the address, not value.
            _out << std::format("    la {}, {}\n", targetReg, _module->globals.at(valID.id).name);
            break;
    }
}

void RISCVEmitter::pushStack(std::size_t id, std::string_view srcReg) {
    _out << std::format("    sw {}, -{}(s0)\n", srcReg, getSlotOffset(id));
}

void RISCVEmitter::loadFromStack(std::size_t id, std::string_view targetReg) {
    _out << std::format("    lw {}, -{}(s0)\n", targetReg, getSlotOffset(id));
}

void RISCVEmitter::emitModule(const ir::LLVMModule& module) {
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

    // stack frame must have slots for each instruction
    // currently we hard code stackItemSize
    // additionally, sp must be 16-byte aligned
    const size_t numInstrs = func.instructions.size();
    const size_t dataSize = numInstrs * stackItemSize;
    size_t totalFrameSize = (prologueSize + dataSize + 15) & ~15;  // NOLINT

    // prologue
    _out << std::format("    addi sp, sp, -{}\n", totalFrameSize);   // allocate stack
    _out << std::format("    sw ra, {}(sp)\n", totalFrameSize - 4);  // save ra
    _out << std::format("    sw s0, {}(sp)\n", totalFrameSize - 8);  // save old frame pointer
    _out << std::format("    addi s0, sp, {}\n", totalFrameSize);    // set frame pointer

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
    _out << std::format("    lw ra, {}(sp)\n", totalFrameSize - 4);  // restore old frame pointer
    _out << std::format("    lw s0, {}(sp)\n", totalFrameSize - 8);  // restore old frame pointer
    _out << std::format("    addi sp, sp, {}\n", totalFrameSize);    // deallocate frame
    _out << "    ret\n";

    _out << std::format("    .size {0}, .-{0}\n", func.name);
}

void RISCVEmitter::emitInstruction(ir::LLVMInstrID instrID) {
    const ir::LLVMInstruction& instr = ir::getInstr(*_curFunc, instrID);

    switch (instr.opcode) {
        case ir::LLVMOpcode::ADD:
        case ir::LLVMOpcode::SUB:
        case ir::LLVMOpcode::MUL:
            loadValue(instr.operands.at(0), "t0");
            loadValue(instr.operands.at(1), "t1");
            _out << std::format("    {} t2, t0, t1\n", ir::printOpcode(instr.opcode));
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::FADD:
        case ir::LLVMOpcode::FSUB:
        case ir::LLVMOpcode::FMUL:
        case ir::LLVMOpcode::FDIV: {
            std::string precision;
            if (instr.type.type == ir::LLVMBasicType::FLOAT) {
                precision = "s";
            } else if (instr.type.type == ir::LLVMBasicType::DOUBLE) {
                precision = "d";
            } else {
                assert(false && "Invalid type for floating point arithmetic.");
            };
            loadValue(instr.operands.at(0), "ft0");
            loadValue(instr.operands.at(1), "ft1");
            _out << std::format("    {}.{} ft2, ft0, ft1\n", ir::printOpcode(instr.opcode),
                                precision);
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
                    assert(false && "Unimplemented ICMP instruction in RV backend");
            }
            pushStack(instrID.id, "t2");
            break;
        case ir::LLVMOpcode::ALLOCA:
            // alloca does not generate any code
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
            break;
        case ir::LLVMOpcode::BR:
            // unconditional jump
            if (instr.operands.size() == 1) {
                _out << std::format("    j .L_{}_{}\n", _curFunc->name,
                                    ir::getValueLabel(*_module, *_curFunc, instr.operands.at(0)));
            } else {
                loadValue(instr.operands.at(0), "t0");
                _out << std::format("    bnez t0, .L_{}_{}\n", _curFunc->name,
                                    ir::getValueLabel(*_module, *_curFunc, instr.operands.at(1)));
                _out << std::format("    j .L_{}_{}\n", _curFunc->name,
                                    ir::getValueLabel(*_module, *_curFunc, instr.operands.at(2)));
            }
            break;
        case ir::LLVMOpcode::CALL:
            _out << std::format("    call {}\n", "");  // TODO
            break;
        case ir::LLVMOpcode::RET:
            loadValue(instr.operands.at(0), "a0");
            _out << std::format("    j .L_{}_epilogue\n", _curFunc->name);
            break;
        case ir::LLVMOpcode::FCMP:
        case ir::LLVMOpcode::GETELEMENTPTR:
            break;
    }
}

}  // namespace thogcc::codegen

// NOLINTEND(*-magic-numbers)
