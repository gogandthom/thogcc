#include <fstream>
#include <iostream>
#include <memory>
#include <utility>

#include "CScanner.h"
#include "TypedefTable.h"
#include "ast/Node.h"
#include "cli.h"
#include "parser.tab.hpp"

namespace thogcc {

// NOLINTNEXTLINE(misc-use-internal-linkage)
std::unique_ptr<ast::Node> ParseC(std::ifstream& file, const CommandLineArgs& args) {
    CScanner scanner(file, std::cerr);

    TypedefTable typedefTable;
    yy::g_root = nullptr;
    yy::parser parse(scanner, typedefTable);

#ifndef NDEBUG
    // parse.trace only set for debug builds
    parse.set_debug_level((int)args.verbose);
#else
    (void)args;
#endif

    parse();

    return std::move(yy::g_root);
}

}  // namespace thogcc
