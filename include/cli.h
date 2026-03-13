#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace thogcc {

struct CommandLineArgs {
    std::string srcPath;
    std::string destPath;
    bool verbose = false;
    bool printGraph = false;
};

CommandLineArgs parseArgs(const std::vector<std::string_view> &args);

}  // namespace thogcc
