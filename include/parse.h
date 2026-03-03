#pragma once

#include <memory>

#include "ast/Node.h"

namespace thogcc {

std::unique_ptr<ast::Node> ParseC(std::ifstream& file);

}  // namespace thogcc
