#pragma once

// Include guard in FlexLexer.h is broken
#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

#include <istream>
#include <ostream>

#include "TypedefTable.h"
#include "parser.tab.hpp"

namespace thogcc {

class CScanner : public yyFlexLexer {
   public:
    CScanner(std::istream& in, std::ostream& out) : yyFlexLexer(in, out){};

    // Suppresses warning about hidden overloaded virtual function
    [[deprecated("Do not call yylex without arguments!")]] int yylex() {
        return 1;
    };

    yy::parser::symbol_type yylex(
        thogcc::TypedefTable& typedefTable);  // implementation in generated lexer.yy.cpp
};

}  // namespace thogcc
