#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

#include "ast/Node.h"
#include "cli.h"
#include "errors/errors.h"
#include "ir/IREmitter.h"
#include "parse.h"
#include "visitors/IRGenVisitor.h"
#include "visitors/PrintVisitor.h"

int main(int argc, char** argv) {
    try {
        // Parse args
        const std::vector<std::string_view> rawArgs(argv + 1, argv + argc);
        const thogcc::CommandLineArgs args = thogcc::parseArgs(rawArgs);

        // Open source file
        std::ifstream input(args.srcPath);
        if (!input.is_open()) {
            throw thogcc::errors::CommandLineError(
                std::format("Couldn't open input file: {}", args.srcPath));
        }

        // Parse source file
        auto root = thogcc::ParseC(input, args);

        // Print AST
        if (args.printGraph) {
            auto printer = thogcc::visitors::PrintVisitor(std::cout);
            root->accept(printer);
        }

        // Generate IR
        auto irGenerator = thogcc::visitors::IRGenVisitor(args.srcPath);
        root->accept(irGenerator);

        if (!args.llvmDestPath.empty()) {
            std::ofstream llvmOut(args.llvmDestPath);
            if (!input.is_open()) {
                throw thogcc::errors::CommandLineError(
                    std::format("Couldn't open llvm output file: {}", args.llvmDestPath));
            }
            thogcc::ir::IREmitter llvmEmitter(llvmOut);

            llvmEmitter.emit(irGenerator.getModule());
        }

    } catch (const std::exception& e) {
        std::cerr << "thogcc: " << e.what() << '\n';
        return 1;
    }
}
