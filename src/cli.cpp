#include "cli.h"

#include <string>
#include <string_view>
#include <vector>

#include "errors/errors.h"

namespace thogcc {

CommandLineArgs parseArgs(const std::vector<std::string_view>& args) {
    CommandLineArgs res;
    auto it = args.begin();

    while (it != args.end()) {
        const std::string_view arg = *it;

        if (arg == "-S") {
            if (++it == args.end()) throw errors::CommandLineError("-S requires a filename");
            res.srcPath = *it;
        } else if (arg == "-o") {
            if (++it == args.end()) throw errors::CommandLineError("-o requires a filename");
            res.destPath = *it;
        } else if (arg == "-v" || arg == "--verbose") {
            res.verbose = true;
        } else if (arg == "-g" || arg == "--graph") {
            res.printGraph = true;
        } else if (arg == "-l" || arg == "--emit-llvm") {
            if (++it == args.end()) {
                throw errors::CommandLineError("--emit-llvm requires a filename");
            }
            res.llvmDestPath = *it;
        } else {
            throw errors::CommandLineError("Unknown argument: " + std::string(arg));
        }

        ++it;
    }

    return res;
};

}  // namespace thogcc
