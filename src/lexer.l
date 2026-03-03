%option c++
%option noyywrap
%option yylineno

%{
    // A lot of this lexer is based off the ANSI C grammar:
    // https://www.lysator.liu.se/c/ANSI-C-grammar-l.html

    // Avoids error "error: `fileno' was not declared in this scope"
    extern "C" int fileno(FILE *stream);

    #include "parser.tab.hpp"

    using token = yy::parser::token;

    // Suppress warning about unused function
    [[maybe_unused]] static void yyunput (int c, char * yy_bp);
%}

D   [0-9]
L   [a-zA-Z_]
H   [a-fA-F0-9]
E   [Ee][+-]?{D}+
FS  (f|F|l|L)
IS  (u|U|l|L)*

%x COMMENT

%%
"/*"        { BEGIN(COMMENT); }
<COMMENT>{
.|\n        { /* eat */ }
"*/"        { BEGIN(INITIAL); }
}

"auto"      { return(token::AUTO); }
"break"     { return(token::BREAK); }
"case"      { return(token::CASE); }
"char"      { return(token::CHAR); }
"const"     { return(token::CONST); }
"continue"  { return(token::CONTINUE); }
"default"   { return(token::DEFAULT); }
"do"        { return(token::DO); }
"double"    { return(token::DOUBLE); }
"else"      { return(token::ELSE); }
"enum"      { return(token::ENUM); }
"extern"    { return(token::EXTERN); }
"float"     { return(token::FLOAT); }
"for"       { return(token::FOR); }
"goto"      { return(token::GOTO); }
"if"        { return(token::IF); }
"int"       { return(token::INT); }
"long"      { return(token::LONG); }
"register"  { return(token::REGISTER); }
"return"    { return(token::RETURN); }
"short"     { return(token::SHORT); }
"signed"    { return(token::SIGNED); }
"sizeof"    { return(token::SIZEOF); }
"static"    { return(token::STATIC); }
"struct"    { return(token::STRUCT); }
"switch"    { return(token::SWITCH); }
"typedef"   { return(token::TYPEDEF); }
"union"     { return(token::UNION); }
"unsigned"  { return(token::UNSIGNED); }
"void"      { return(token::VOID); }
"volatile"  { return(token::VOLATILE); }
"while"     { return(token::WHILE); }

{L}({L}|{D})*       { return(typedefTable.isType(yytext) ? yy::parser::make_TYPE_NAME(yytext) : yy::parser::make_IDENTIFIER(yytext)); }

0[xX]{H}+{IS}?      { return(yy::parser::make_INT_CONSTANT((int)strtol(yytext, nullptr, 0)));
                      /* TODO what in gods name is this shit. uints and ints are longs cast to ints????? */ }
0{D}+{IS}?          { return(yy::parser::make_INT_CONSTANT((int)strtol(yytext, nullptr, 0))); }
{D}+{IS}?           { return(yy::parser::make_INT_CONSTANT((int)strtol(yytext, nullptr, 0))); }
L?'(\\.|[^\\'])+'   { return(yy::parser::make_INT_CONSTANT((int)strtol(yytext, nullptr, 0))); }

{D}+{E}{FS}?            { return(yy::parser::make_FLOAT_CONSTANT(strtod(yytext, nullptr))); }
{D}*"."{D}+({E})?{FS}?  { return(yy::parser::make_FLOAT_CONSTANT(strtod(yytext, nullptr))); }
{D}+"."{D}*({E})?{FS}?  { return(yy::parser::make_FLOAT_CONSTANT(strtod(yytext, nullptr))); }

L?\"(\\.|[^\\"])*\"     {/* TODO process string literal */; return(STRING_LITERAL); }

"..."       { return(token::ELLIPSIS); }
">>="       { return(token::RIGHT_ASSIGN); }
"<<="       { return(token::LEFT_ASSIGN); }
"+="        { return(token::ADD_ASSIGN); }
"-="        { return(token::SUB_ASSIGN); }
"*="        { return(token::MUL_ASSIGN); }
"/="        { return(token::DIV_ASSIGN); }
"%="        { return(token::MOD_ASSIGN); }
"&="        { return(token::AND_ASSIGN); }
"^="        { return(token::XOR_ASSIGN); }
"|="        { return(token::OR_ASSIGN); }
">>"        { return(token::RIGHT_OP); }
"<<"        { return(token::LEFT_OP); }
"++"        { return(token::INC_OP); }
"--"        { return(token::DEC_OP); }
"->"        { return(token::PTR_OP); }
"&&"        { return(token::AND_OP); }
"||"        { return(token::OR_OP); }
"<="        { return(token::LE_OP); }
">="        { return(token::GE_OP); }
"=="        { return(token::EQ_OP); }
"!="        { return(token::NE_OP); }
";"         { return(';'); }
("{"|"<%")  { return('{'); }
("}"|"%>")  { return('}'); }
","         { return(','); }
":"         { return(':'); }
"="         { return('='); }
"("         { return('('); }
")"         { return(')'); }
("["|"<:")  { return('['); }
("]"|":>")  { return(']'); }
"."         { return('.'); }
"&"         { return('&'); }
"!"         { return('!'); }
"~"         { return('~'); }
"-"         { return('-'); }
"+"         { return('+'); }
"*"         { return('*'); }
"/"         { return('/'); }
"%"         { return('%'); }
"<"         { return('<'); }
">"         { return('>'); }
"^"         { return('^'); }
"|"         { return('|'); }
"?"         { return('?'); }

[ \a\b\t\v\f\n\r]   { /* ignore new lines and special sequences */ }
.                   { std::cerr << "Unknown token: " << yytext << std::endl; return(UNKNOWN); }

%%
