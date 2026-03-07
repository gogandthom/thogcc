#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

#include "cli.h"
#include "errors/errors.h"
#include "parse.h"

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
        thogcc::ParseC(input, args);
    } catch (const std::exception& e) {
        std::cerr << "thogcc: " << e.what() << '\n';
        return 1;
    }
}
