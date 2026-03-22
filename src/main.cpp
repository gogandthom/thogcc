#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

#include "ast/Node.h"
#include "cli.h"
#include "codegen/RISCVEmitter.h"
#include "errors/errors.h"
#include "ir/IREmitter.h"
#include "ir/llvm.h"
#include "parse.h"
#include "types/SymbolTable.h"
#include "visitors/IRGenVisitor.h"
#include "visitors/PrintVisitor.h"
#include "visitors/SemaVisitor.h"

int main(int argc, char** argv) {
    try {
        // Parse args
        const std::vector<std::string_view> rawArgs(argv + 1, argv + argc);  // NOLINT
        const thogcc::CommandLineArgs args = thogcc::parseArgs(rawArgs);

        // Open source file
        const std::filesystem::path srcPath(args.srcPath);
        std::ifstream input(srcPath);
        if (!input.is_open()) {
            throw thogcc::errors::CommandLineError(
                std::format("Couldn't open input file: {}", args.srcPath));
        }

        // Parse source file
        auto root = thogcc::ParseC(input, args);

        // SEMAAAAA
        thogcc::types::SymbolTable table{};
        auto sema = thogcc::visitors::SemaVisitor(table);
        root->accept(sema);

        // Print AST
        if (args.printGraph) {
            auto printer = thogcc::visitors::PrintVisitor(std::cout);
            root->accept(printer);
        }

        // Generate IR
        thogcc::ir::LLVMModule llvmModule;
        llvmModule.srcFileName = srcPath.filename();
        auto irGenerator = thogcc::visitors::IRGenVisitor(llvmModule);
        root->accept(irGenerator);

        if (!args.llvmDestPath.empty()) {
            std::ofstream llvmOut(args.llvmDestPath);
            if (!input.is_open()) {
                throw thogcc::errors::CommandLineError(
                    std::format("Couldn't open llvm output file: {}", args.llvmDestPath));
            }
            thogcc::ir::IREmitter llvmEmitter(llvmOut);
            llvmEmitter.emit(llvmModule);
        }

        if (!args.destPath.empty()) {
            std::ofstream riscOut(args.destPath);
            if (!riscOut.is_open()) {
                throw thogcc::errors::CommandLineError(
                    std::format("Couldn't open RISCV assembly output file: {}", args.destPath));
            }
            thogcc::codegen::RISCVEmitter riscv(riscOut);
            riscv.emit(llvmModule);
        }
    } catch (const std::exception& e) {
        std::cerr << "thogcc: " << e.what() << '\n';
        return 1;
    }
}
