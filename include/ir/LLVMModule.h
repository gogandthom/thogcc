#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "ir/LLVMType.h"

namespace thogcc::ir {

enum class LLVMValueKind : std::uint8_t {
    PARAM,
    INSTR,
    CONST,
    BLOCK,
    GLOBAL,
};

struct LLVMValueID {
    LLVMValueKind kind;
    std::size_t id;  // index into pool/'arena'
};

struct LLVMInstrID {
    std::size_t id;
};

struct LLVMBlockID {
    std::size_t id;
};

#define LLVM_OPCODE                              \
    /* binary operations */                      \
    X(ADD, "add")                                \
    X(FADD, "fadd")                              \
    X(SUB, "sub")                                \
    X(FSUB, "fsub")                              \
    X(MUL, "mul")                                \
    X(FMUL, "fmul")                              \
    X(UDIV, "udiv")                              \
    X(SDIV, "sdiv")                              \
    X(FDIV, "fdiv")                              \
    X(UREM, "urem")                              \
    X(SREM, "srem")                              \
                                                 \
    /* compare things */                         \
    X(ICMP, "icmp")                              \
    X(FCMP, "fcmp")                              \
                                                 \
    /* memory */                                 \
    X(ALLOCA, "alloca")                          \
    X(LOAD, "load")                              \
    X(STORE, "store")                            \
    X(GETELEMENTPTR, "getelementptr") /* TODO */ \
                                                 \
    /* control flow */                           \
    X(BR, "br")                                  \
    X(CALL, "call")                              \
    X(RET, "ret")

enum class LLVMOpcode : std::uint8_t {
#define X(VAL, NAME) VAL,
    LLVM_OPCODE
#undef X
};

#define LLVM_CMP_COND \
    X(EQ, "eq")       \
    X(NE, "ne")       \
    X(UGT, "ugt")     \
    X(UGE, "uge")     \
    X(ULT, "ult")     \
    X(SGT, "sgt")     \
    X(SGE, "sge")     \
    X(SLT, "slt")     \
    X(SLE, "sle")

enum class LLVMCmpCond : std::uint8_t {
#define X(VAL, NAME) VAL,
    LLVM_CMP_COND
#undef X
};

/// helper to get opcode
inline std::string_view printOpcode(const LLVMOpcode& op) {
    switch (op) {
#define X(VAL, NAME)      \
    case LLVMOpcode::VAL: \
        return NAME;
        LLVM_OPCODE
#undef X
    }
    assert(false && "Unhandled LLVMOpcode");
    __builtin_unreachable();
};

inline std::string_view printCmpCond(const LLVMCmpCond& op) {
    switch (op) {
#define X(VAL, NAME)       \
    case LLVMCmpCond::VAL: \
        return NAME;
        LLVM_CMP_COND
#undef X
    }
}

/// A set of instructions that runs start to finish without branching
struct LLVMBasicBlock {
    std::string label;
    std::vector<LLVMInstrID> instrIDs;  // indexes into function 'instructions' pool
};

/// A single LLVM instruction
struct LLVMInstruction {
    LLVMOpcode opcode;
    LLVMType type;
    std::vector<LLVMValueID> operands;
    LLVMCmpCond cond;  // only for ICMP
};

// Function parameters (we only support named parameters)
struct LLVMParameter {
    LLVMType type;
    std::string name;
};

/// Constants
struct LLVMConstant {
    LLVMType type;
    std::variant<uint64_t, float, double> value;
};

/// Global variable
struct LLVMGlobal {
    LLVMType type;
    std::string name;
    std::variant<uint64_t, double> initValue;
};

/// A single function.
/// Each LLVMFunction also contains its own instructions, params, consts arenas.
struct LLVMFunction {
    std::string name;
    LLVMType returnType;
    std::vector<LLVMBasicBlock> blocks;

    // value pools
    std::vector<LLVMInstruction> instructions;
    std::vector<LLVMParameter> params;
    std::vector<LLVMConstant> consts;
};

/// The LLVM equivalent of a translational unit
struct LLVMModule {
    std::string srcFileName;
    std::vector<LLVMFunction> functions;
    std::vector<LLVMGlobal> globals;
};

}  // namespace thogcc::ir
