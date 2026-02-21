// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires {
    #include "ast.hpp"
    using namespace ast;

    extern int yylineno;
    extern char* yytext;
    extern Node* g_root;
    extern FILE* yyin;

    int yylex(void);
    void yyerror(const char*);
    int yylex_destroy(void);
}

%define parse.error detailed
%define parse.lac full

// Represents the value associated with any kind of AST node.
%union {
  Node*             node;
  NodeList*         node_list;
  int               number_int;
  double            number_float;
  std::string*      string;
  TypeSpecifier     type_specifier;
  yytokentype       token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token UNKNOWN

%type <node> translation_unit external_declaration function_definition primary_expression postfix_expression argument_expression_list
%type <node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression expression constant_expression declaration init_declarator_list
%type <node> init_declarator struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier enumerator_list enumerator declarator direct_declarator pointer parameter_list parameter_declaration
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement labeled_statement
%type <node> compound_statement declaration_list expression_statement selection_statement iteration_statement jump_statement

%type <node_list> statement_list

%type <string> unary_operator assignment_operator storage_class_specifier

%type <number_int> INT_CONSTANT STRING_LITERAL
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER
%type <type_specifier> type_specifier
// TODO: Make a better type for this (only needed for advanced features)
%type <type_specifier> declaration_specifiers


%start ROOT
%%

ROOT
    : translation_unit { g_root = $1; }

translation_unit
    : external_declaration { $$ = $1; }
    | translation_unit external_declaration
    ;

external_declaration
    : function_definition { $$ = $1; }
    | declaration
    ;

function_definition
    : declaration_specifiers declarator declaration_list compound_statement
    | declaration_specifiers declarator compound_statement {
        $$ = new FunctionDefinition($1, NodePtr($2), NodePtr($3));
    }
    | declarator declaration_list compound_statement
    | declarator compound_statement
    ;


primary_expression
    : IDENTIFIER            { $$ = new IdentifierExpression($1); }
    | INT_CONSTANT          { $$ = new PrimaryExpression($1); }
    | FLOAT_CONSTANT        { $$ = new PrimaryExpression($1); }
    | STRING_LITERAL        { $$ = new PrimaryExpression{$1}; }
    | '(' expression ')'    { $$ = $2; }
    ;

postfix_expression
    : primary_expression                                    { $$ = $1; }
    | postfix_expression '[' expression ']'                 { $$ = new postfix::ArrayAccessExpression($1, $3); }
    | postfix_expression '(' ')'                            { $$ = new postfix::FunctionCallExpression($1); }
    | postfix_expression '(' argument_expression_list ')'   { $$ = new postfix::FunctionCallExpression($1, $3); }
    | postfix_expression '.' IDENTIFIER                     { $$ = new postfix::MemberAccessExpression($1); }
    | postfix_expression PTR_OP IDENTIFIER                  { $$ = new postfix::MemberAccessExpression($1, true); }
    | postfix_expression INC_OP                             { $$ = new postfix::IncDecExpression($1); }
    | postfix_expression DEC_OP                             { $$ = new postfix::IncDecExpression($1, true); }
    ;

argument_expression_list
    : assignment_expression                                 { $$ = new NodeList($1); }
    | argument_expression_list ',' assignment_expression    { $$ = std::make_unique<auto>($1->pushBack($3)); }
    ;

unary_expression
    : postfix_expression                { $$ = $1; }
    | INC_OP unary_expression           { $$ = new prefix::IncDecExpression($2); }
    | DEC_OP unary_expression           { $$ = new prefix::IncDecExpression($2, true); }
    | unary_operator cast_expression    { $$ = new prefix::UnaryOperatorExpression{$1, $2}; }
    | SIZEOF unary_expression           { $$ = new prefix::UnaryOperatorExpression{prefix::UnaryOperatorType::SIZEOF, $2}; }
    | SIZEOF '(' type_name ')'          { $$ = new prefix::UnaryOperatorExpression{prefix::UnaryOperatorType::SIZEOF, $3}; }
    ;

unary_operator
    : '&'   { $$ = prefix::UnaryOperatorType::ADDRESSOF; }
    | '*'   { $$ = prefix::UnaryOperatorType::INDIRECTION; }
    | '+'   { $$ = prefix::UnaryOperatorType::PLUS; }
    | '-'   { $$ = prefix::UnaryOperatorType::MINUS; }
    | '~'   { $$ = prefix::UnaryOperatorType::BITWISE_NOT; }
    | '!'   { $$ = prefix::UnaryOperatorType::LOGICAL_NOT; }
    ;

cast_expression
    : unary_expression                  { $$ = $1; }
    | '(' type_name ')' cast_expression { $$ = new CastExpression($1, $3); }
    ;

multiplicative_expression
    : cast_expression
    | multiplicative_expression '*' cast_expression
    | multiplicative_expression '/' cast_expression
    | multiplicative_expression '%' cast_expression
    ;

additive_expression
    : multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;

shift_expression
    : additive_expression
    | shift_expression LEFT_OP additive_expression
    | shift_expression RIGHT_OP additive_expression
    ;

relational_expression
    : shift_expression
    | relational_expression '<' shift_expression
    | relational_expression '>' shift_expression
    | relational_expression LE_OP shift_expression
    | relational_expression GE_OP shift_expression
    ;

equality_expression
    : relational_expression
    | equality_expression EQ_OP relational_expression
    | equality_expression NE_OP relational_expression
    ;

and_expression
    : equality_expression
    | and_expression '&' equality_expression
    ;

exclusive_or_expression
    : and_expression
    | exclusive_or_expression '^' and_expression
    ;

inclusive_or_expression
    : exclusive_or_expression
    | inclusive_or_expression '|' exclusive_or_expression
    ;

logical_and_expression
    : inclusive_or_expression
    | logical_and_expression AND_OP inclusive_or_expression
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR_OP logical_and_expression
    ;

conditional_expression
    : logical_or_expression
    | logical_or_expression '?' expression ':' conditional_expression
    ;

assignment_expression
    : conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;

assignment_operator
    : '='
    | MUL_ASSIGN
    | DIV_ASSIGN
    | MOD_ASSIGN
    | ADD_ASSIGN
    | SUB_ASSIGN
    | LEFT_ASSIGN
    | RIGHT_ASSIGN
    | AND_ASSIGN
    | XOR_ASSIGN
    | OR_ASSIGN
    ;

expression
    : assignment_expression
    | expression ',' assignment_expression
    ;

constant_expression
    : conditional_expression    { $$ = new ConstantExpression($1); } // Don't change this, see ExpressionUnion.h
    ;

declaration
    : declaration_specifiers ';'
    | declaration_specifiers init_declarator_list ';'
    ;

declaration_specifiers
    : storage_class_specifier
    | storage_class_specifier declaration_specifiers
    | type_specifier { $$ = $1; }
    | type_specifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list ',' init_declarator
    ;

init_declarator
    : declarator
    | declarator '=' initializer
    ;

storage_class_specifier
    : TYPEDEF
    | EXTERN
    | STATIC
    | AUTO
    | REGISTER
    ;

type_specifier
    : VOID
    | CHAR
    | SHORT
    | INT { $$ = TypeSpecifier::INT; }
    | LONG
    | FLOAT
    | DOUBLE
    | SIGNED
    | UNSIGNED
    | struct_specifier
    | enum_specifier
    | TYPE_NAME
    ;

struct_specifier
    : STRUCT IDENTIFIER '{' struct_declaration_list '}'
    | STRUCT '{' struct_declaration_list '}'
    | STRUCT IDENTIFIER
    ;

struct_declaration_list
    : struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
    ;

specifier_qualifier_list
    : type_specifier specifier_qualifier_list
    | type_specifier
    ;

struct_declarator_list
    : struct_declarator
    | struct_declarator_list ',' struct_declarator
    ;

struct_declarator
    : declarator
    | ':' constant_expression
    | declarator ':' constant_expression
    ;

enum_specifier
    : ENUM '{' enumerator_list '}'
    | ENUM IDENTIFIER '{' enumerator_list '}'
    | ENUM IDENTIFIER
    ;

enumerator_list
    : enumerator
    | enumerator_list ',' enumerator
    ;

enumerator
    : IDENTIFIER
    | IDENTIFIER '=' constant_expression
    ;

declarator
    : pointer direct_declarator
    | direct_declarator { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER {
        $$ = new Identifier(std::move(*$1));
        delete $1;
    }
    | '(' declarator ')'
    | direct_declarator '[' constant_expression ']'
    | direct_declarator '[' ']'
    | direct_declarator '(' parameter_list ')'
    | direct_declarator '(' identifier_list ')'
    | direct_declarator '(' ')' {
        $$ = new DirectDeclarator(NodePtr($1));
    }
    ;

pointer
    : '*'
    | '*' pointer
    ;

parameter_list
    : parameter_declaration
    | parameter_list ',' parameter_declaration
    ;

parameter_declaration
    : declaration_specifiers declarator
    | declaration_specifiers abstract_declarator
    | declaration_specifiers
    ;

identifier_list
    : IDENTIFIER
    | identifier_list ',' IDENTIFIER
    ;

type_name
    : specifier_qualifier_list
    | specifier_qualifier_list abstract_declarator
    ;

abstract_declarator
    : pointer
    | direct_abstract_declarator
    | pointer direct_abstract_declarator
    ;

direct_abstract_declarator
    : '(' abstract_declarator ')'
    | '[' ']'
    | '[' constant_expression ']'
    | direct_abstract_declarator '[' ']'
    | direct_abstract_declarator '[' constant_expression ']'
    | '(' ')'
    | '(' parameter_list ')'
    | direct_abstract_declarator '(' ')'
    | direct_abstract_declarator '(' parameter_list ')'
    ;

initializer
    : assignment_expression
    | '{' initializer_list '}'
    | '{' initializer_list ',' '}'
    ;

initializer_list
    : initializer
    | initializer_list ',' initializer
    ;

statement
    : labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement { $$ = $1; }
    ;

labeled_statement
    : IDENTIFIER ':' statement
    | CASE constant_expression ':' statement
    | DEFAULT ':' statement
    ;

compound_statement
    : '{' '}' {
        // TODO: correct this
        $$ = nullptr;
    }
    | '{' statement_list '}' {
        $$ = $2;
    }
    | '{' declaration_list '}' {
        // TODO: correct this
        $$ = nullptr;
    }
    | '{' declaration_list statement_list '}'  {
        // TODO: correct this
        $$ = nullptr;
    }
    ;

declaration_list
    : declaration
    | declaration_list declaration
    ;

statement_list
    : statement { $$ = new NodeList(NodePtr($1)); }
    | statement_list statement { $1->PushBack(NodePtr($2)); $$=$1; }
    ;

expression_statement
    : ';'
    | expression ';' { $$ = $1; }
    ;

selection_statement
    : IF '(' expression ')' statement
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;

iteration_statement
    : WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_statement expression_statement ')' statement
    | FOR '(' expression_statement expression_statement expression ')' statement
    ;

jump_statement
    : GOTO IDENTIFIER ';'
    | CONTINUE ';'
    | BREAK ';'
    | RETURN ';' {
        $$ = new ReturnStatement(nullptr);
    }
    | RETURN expression ';' {
        $$ = new ReturnStatement(NodePtr($2));
    }
    ;



%%

void yyerror (const char *s)
{
    std::cerr << "Error: " << s << " at line " << yylineno;
    std::cerr << " near '" << yytext << "'" << std::endl;
    std::exit(1);
}

Node* g_root;

NodePtr ParseAST(std::string file_name)
{
    yyin = fopen(file_name.c_str(), "r");
    if (yyin == nullptr) {
        std::cerr << "Couldn't open input file: " << file_name << std::endl;
        std::exit(1);
    }

    g_root = nullptr;
    yyparse();

    fclose(yyin);
    yylex_destroy();

    return NodePtr(g_root);
}
