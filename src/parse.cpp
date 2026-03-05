#include <fstream>
#include <iostream>
#include <memory>
#include <utility>

#include "CScanner.h"
#include "TypedefTable.h"
#include "ast/Node.h"
#include "parser.tab.hpp"

namespace thogcc {

std::unique_ptr<ast::Node> ParseC(std::ifstream& file) {
    CScanner scanner(file, std::cerr);

    TypedefTable typedefTable;
    yy::g_root = nullptr;
    yy::parser parse(scanner, typedefTable);

    parse();

    return std::move(yy::g_root);
}

}  // namespace thogcc
