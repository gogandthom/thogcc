#pragma once

#include <fstream>
#include <memory>

#include "ast/Node.h"
#include "cli.h"

namespace thogcc {

std::unique_ptr<ast::Node> ParseC(std::ifstream& file, const CommandLineArgs& args);

}  // namespace thogcc
