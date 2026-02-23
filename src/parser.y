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

// Do modern cpp things
%skeleton "lalr1.cc"
%require "3.2"

// Modern replacement for %union
%define api.value.type variant

// type safe %token
%define api.token.constructor

%define parse.error detailed
%define parse.lac full

%token <std::string> IDENTIFIER
%token <int> INT_CONSTANT STRING_LITERAL // TODO fix
%token <double> FLOAT_CONSTANT
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token UNKNOWN

%type <std::unique_ptr<Node>> translation_unit external_declaration function_definition primary_expression postfix_expression argument_expression_list
%type <std::unique_ptr<Node>> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <std::unique_ptr<Node>> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <std::unique_ptr<Node>> conditional_expression assignment_expression expression constant_expression declaration init_declarator_list
%type <std::unique_ptr<Node>> init_declarator struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <std::unique_ptr<Node>> struct_declarator enum_specifier enumerator_list enumerator declarator direct_declarator pointer parameter_list parameter_declaration
%type <std::unique_ptr<Node>> identifier_list type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement labelled_statement
%type <std::unique_ptr<Node>> compound_statement declaration_list expression_statement selection_statement iteration_statement jump_statement

%type <std::unique_ptr<NodeList>> statement_list

%type <expression::prefix::UnaryOperatorType> unary_operator
%type <expression::AssignmentExpressionType> assignment_operator
%type <std::string> storage_class_specifier // TODO use correct type

%type <TypeSpecifier> type_specifier
// TODO: Make a better type for this (only needed for advanced features)
%type <TypeSpecifier> declaration_specifiers


%start ROOT
%%

ROOT
    : translation_unit { g_root = std::move($1); }

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
    : IDENTIFIER            { $$ = std::make_unique<IdentifierExpression>($1); }
    | INT_CONSTANT          { $$ = std::make_unique<PrimaryExpression>($1); }
    | FLOAT_CONSTANT        { $$ = std::make_unique<PrimaryExpression>($1); }
    | STRING_LITERAL        { $$ = std::make_unique<PrimaryExpression>($1); } // TODO change? also ownership?
    | '(' expression ')'    { $$ = std::move($2); }
    ;

postfix_expression
    : primary_expression                                    { $$ = std::move($1); }
    | postfix_expression '[' expression ']'                 { $$ = std::make_unique<postfix::ArrayAccessExpression>(std::move($1), std::move($3)); }
    | postfix_expression '(' ')'                            { $$ = std::make_unique<postfix::FunctionCallExpression>(std::move($1)); }
    | postfix_expression '(' argument_expression_list ')'   { $$ = std::make_unique<postfix::FunctionCallExpression>(std::move($1), std::move($3)); }
    | postfix_expression '.' IDENTIFIER                     { $$ = std::make_unique<postfix::MemberAccessExpression>(std::move($1)); }
    | postfix_expression PTR_OP IDENTIFIER                  { $$ = std::make_unique<postfix::MemberAccessExpression>(std::move($1), true); }
    | postfix_expression INC_OP                             { $$ = std::make_unique<postfix::IncDecExpression>(std::move($1)); }
    | postfix_expression DEC_OP                             { $$ = std::make_unique<postfix::IncDecExpression>(std::move($1), true); }
    ;

argument_expression_list
    : assignment_expression                                 { $$ = std::make_unique<NodeList>($1); }
    | argument_expression_list ',' assignment_expression    { $$ = std::move($1->pushBack($3)); }
    ;

unary_expression
    : postfix_expression                { $$ = std::move($1); }
    | INC_OP unary_expression           { $$ = std::make_unique<prefix::IncDecExpression>(std::move($2)); }
    | DEC_OP unary_expression           { $$ = std::make_unique<prefix::IncDecExpression>(std::move($2), true); }
    | unary_operator cast_expression    { $$ = std::make_unique<prefix::UnaryOperatorExpression>($1, std::move($2)); }
    | SIZEOF unary_expression           { $$ = std::make_unique<prefix::UnaryOperatorExpression>(prefix::UnaryOperatorType::SIZEOF, std::move($2)); }
    | SIZEOF '(' type_name ')'          { $$ = std::make_unique<prefix::UnaryOperatorExpression>(prefix::UnaryOperatorType::SIZEOF, std::move($3)); }
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
    : unary_expression                  { $$ = std::move($1); }
    | '(' type_name ')' cast_expression { $$ = std::make_unique<CastExpression>(std::move($1), std::move($3)); }
    ;

multiplicative_expression
    : cast_expression                               { $$ = std::move($1); }
    | multiplicative_expression '*' cast_expression { $$ = std::make_unique<MultiplicativeExpression>(std::move($1), std::move($3), expressions::MultiplicativeExpressionType::MUL); }
    | multiplicative_expression '/' cast_expression { $$ = std::make_unique<MultiplicativeExpression>(std::move($1), std::move($3), expressions::MultiplicativeExpressionType::DIV); }
    | multiplicative_expression '%' cast_expression { $$ = std::make_unique<MultiplicativeExpression>(std::move($1), std::move($3), expressions::MultiplicativeExpressionType::REM); }
    ;

additive_expression
    : multiplicative_expression                         { $$ = std::move($1); }
    | additive_expression '+' multiplicative_expression { $$ = std::make_unique<AdditiveExpression>(std::move($1), std::move($3)); }
    | additive_expression '-' multiplicative_expression { $$ = std::make_unique<AdditiveExpression>(std::move($1), std::move($3), true); }
    ;

shift_expression
    : additive_expression                           { $$ = std::move($1); }
    | shift_expression LEFT_OP additive_expression  { $$ = std::make_unique<ShiftExpression>(std::move($1), std::move($3)); }
    | shift_expression RIGHT_OP additive_expression { $$ = std::make_unique<ShiftExpression>(std::move($1), std::move($3), true); }
    ;

relational_expression
    : shift_expression                              { $$ = std::move($1); }
    | relational_expression '<' shift_expression    { $$ = std::make_unique<RelationalExpression>(std::move($1), std::move($3), expressions::RelationalExpressionType::L); }
    | relational_expression '>' shift_expression    { $$ = std::make_unique<RelationalExpression>(std::move($1), std::move($3), expressions::RelationalExpressionType::G); }
    | relational_expression LE_OP shift_expression  { $$ = std::make_unique<RelationalExpression>(std::move($1), std::move($3), expressions::RelationalExpressionType::LE); }
    | relational_expression GE_OP shift_expression  { $$ = std::make_unique<RelationalExpression>(std::move($1), std::move($3), expressions::RelationalExpressionType::GE); }
    ;

equality_expression
    : relational_expression                             { $$ = std::move($1); }
    | equality_expression EQ_OP relational_expression   { $$ = std::make_unique<EqualityExpression>(std::move($1), std::move($3)); }
    | equality_expression NE_OP relational_expression   { $$ = std::make_unique<EqualityExpression>(std::move($1), std::move($3), true); }
    ;

and_expression
    : equality_expression                       { $$ = std::move($1); }
    | and_expression '&' equality_expression    { $$ = std::make_unique<BitwiseExpression>(std::move($1), std::move($3), expressions::BitwiseExpressionType::AND); }
    ;

exclusive_or_expression
    : and_expression                                { $$ = std::move($1); }
    | exclusive_or_expression '^' and_expression    { $$ = std::make_unique<BitwiseExpression>(std::move($1), std::move($3), expressions::BitwiseExpressionType::XOR); }
    ;

inclusive_or_expression
    : exclusive_or_expression                               { $$ = std::move($1); }
    | inclusive_or_expression '|' exclusive_or_expression   { $$ = std::make_unique<BitwiseExpression>(std::move($1), std::move($3), expressions::BitwiseExpressionType::OR); }
    ;

logical_and_expression
    : inclusive_or_expression                               { $$ = std::move($1); }
    | logical_and_expression AND_OP inclusive_or_expression { $$ = std::make_unique<LogicalExpression>(std::move($1), std::move($3), expressions::LogicalExpressionType::AND); }
    ;

logical_or_expression
    : logical_and_expression                                { $$ = std::move($1); }
    | logical_or_expression OR_OP logical_and_expression    { $$ = std::make_unique<LogicalExpression>(std::move($1), std::move($3), expressions::LogicalExpressionType::OR); }
    ;

conditional_expression
    : logical_or_expression                                             { $$ = std::move($1); }
    | logical_or_expression '?' expression ':' conditional_expression   { $$ = std::make_unique<ConditionalExpression>(std::move($1), std::move($2), std::move($3))}
    ;

assignment_expression
    : conditional_expression                                        { $$ = std::move($1); }
    | unary_expression assignment_operator assignment_expression    { $$ = std::make_unique<AssignmentExpression>(std::move($1), $2, std::move($3)); }
    ;

assignment_operator
    : '='           { $$ = expression::AssignmentExpressionType::ASSIGN; }
    | MUL_ASSIGN    { $$ = expression::AssignmentExpressionType::MUL_ASSIGN; }
    | DIV_ASSIGN    { $$ = expression::AssignmentExpressionType::DIV_ASSIGN; }
    | MOD_ASSIGN    { $$ = expression::AssignmentExpressionType::MOD_ASSIGN; }
    | ADD_ASSIGN    { $$ = expression::AssignmentExpressionType::ADD_ASSIGN; }
    | SUB_ASSIGN    { $$ = expression::AssignmentExpressionType::SUB_ASSIGN; }
    | LEFT_ASSIGN   { $$ = expression::AssignmentExpressionType::LEFT_ASSIGN; }
    | RIGHT_ASSIGN  { $$ = expression::AssignmentExpressionType::RIGHT_ASSIGN; }
    | AND_ASSIGN    { $$ = expression::AssignmentExpressionType::AND_ASSIGN; }
    | XOR_ASSIGN    { $$ = expression::AssignmentExpressionType::XOR_ASSIGN; }
    | OR_ASSIGN     { $$ = expression::AssignmentExpressionType::OR_ASSIGN; }
    ;

expression
    : assignment_expression                 { $$ = std::make_unique<ListExpression>(std::move($1)); }
    | expression ',' assignment_expression  { $$ = $1->pushBack(std::move($3)); }
    ;

constant_expression
    : conditional_expression    { $$ = std::make_unique<ConstantExpression>(std::move($1)); } // Don't change this, see ExpressionUnion.h
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

std::unique_ptr<Node> g_root;

std::unique_ptr<Node> ParseAST(const std::string& file_name)
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

    return std::move(g_root);
}
