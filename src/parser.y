// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires {
    #include "ast.hpp"
    using namespace ast;

    extern int yylineno;
    extern char* yytext;
    extern std::unique_ptr<Node> g_root;
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

%precedence LOWER_THAN_ELSE
%precedence ELSE

%type <std::unique_ptr<Node>> type_name
%type <std::unique_ptr<Node>> declaration_specifiers // TODO: Make a better type for this (only needed for advanced features)

// Top level shite
%type <std::unique_ptr<Node>> translation_unit

// Expressions
%type <std::unique_ptr<ExpressionBase>> primary_expression postfix_expression unary_expression cast_expression
%type <std::unique_ptr<ExpressionBase>> multiplicative_expression additive_expression shift_expression relational_expression equality_expression
%type <std::unique_ptr<ExpressionBase>> and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <std::unique_ptr<ExpressionBase>> conditional_expression assignment_expression expression initializer
%type <std::unique_ptr<ConstantExpression>> constant_expression
%type <std::unique_ptr<NodeList<ExpressionBase>>> initializer_list argument_expression_list

// Statements
%type <std::unique_ptr<StatementBase>> compound_statement expression_statement selection_statement iteration_statement jump_statement labelled_statement statement
%type <std::unique_ptr<NodeList<StatementBase>>> statement_list statement_list_opt

// Declarators
%type <std::unique_ptr<DeclaratorBase>> direct_declarator direct_abstract_declarator abstract_declarator declarator init_declarator
%type <std::unique_ptr<StructDeclarator>> struct_declarator
%type <std::unique_ptr<NodeList<DeclaratorBase>>> init_declarator_list
%type <std::unique_ptr<NodeList<IdentifierDeclarator>>> identifier_list
%type <std::unique_ptr<NodeList<StructDeclarator>>> struct_declarator_list

// Declarations (derived from Declarators??)
%type <std::unique_ptr<DeclarationBase>> declaration external_declaration
%type <std::unique_ptr<StructDeclaration>> struct_declaration
%type <std::unique_ptr<EnumeratorValueDeclaration>> enumerator
%type <std::unique_ptr<ParameterDeclaration>> parameter_declaration
%type <std::unique_ptr<FunctionDefinition>> function_definition
%type <std::unique_ptr<NodeList<DeclarationBase>>> declaration_list declaration_list_opt
%type <std::unique_ptr<NodeList<StructDeclaration>>> struct_declaration_list
%type <std::unique_ptr<NodeList<EnumeratorValueDeclaration>>> enumerator_list
%type <std::unique_ptr<NodeList<ParameterDeclaration>>> parameter_list

// Enumerators and structs
%type <std::unique_ptr<Enumerator>> enum_specifier
%type <std::unique_ptr<Struct>> struct_specifier

// Enums for types and shit
%type <ast::StorageClassSpecifier> storage_class_specifier
%type <expression::prefix::UnaryOperatorType> unary_operator
%type <expression::AssignmentExpressionType> assignment_operator

%type <TypeSpecifier> type_specifier
%type <std::unique_ptr<NodeList<TypeSpecifier>>> specifier_qualifier_list

%type <int> pointer


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
    | STRING_LITERAL        { $$ = std::make_unique<PrimaryExpression>($1); }
    | '(' expression ')'    { $$ = std::move($2); }
    ;

postfix_expression
    : primary_expression                                    { $$ = std::move($1); }
    | postfix_expression '[' expression ']'                 { $$ = std::make_unique<postfix::ArrayAccessExpression>(std::move($1), std::move($3)); }
    | postfix_expression '(' ')'                            { $$ = std::make_unique<postfix::FunctionCallExpression>(std::move($1)); }
    | postfix_expression '(' argument_expression_list ')'   { $$ = std::make_unique<postfix::FunctionCallExpression>(std::move($1), std::move($3)); }
    | postfix_expression '.' IDENTIFIER                     { $$ = std::make_unique<postfix::MemberAccessExpression>(std::move($1), $3); }
    | postfix_expression PTR_OP IDENTIFIER                  { $$ = std::make_unique<postfix::MemberAccessExpression>(std::move($1), $3, true); }
    | postfix_expression INC_OP                             { $$ = std::make_unique<IncDecExpression>(std::move($1)); }
    | postfix_expression DEC_OP                             { $$ = std::make_unique<IncDecExpression>(std::move($1), true); }
    ;

argument_expression_list
    : assignment_expression                                 { $$ = std::make_unique<NodeList<ExpressionBase>>($1); }
    | argument_expression_list ',' assignment_expression    { $1->pushBack($3); $$ = std::move($1); }
    ;

unary_expression
    : postfix_expression                { $$ = std::move($1); }
    | INC_OP unary_expression           { $$ = std::make_unique<IncDecExpression>(std::move($2), true); }
    | DEC_OP unary_expression           { $$ = std::make_unique<IncDecExpression>(std::move($2), true, true); }
    | unary_operator cast_expression    { $$ = std::make_unique<prefix::UnaryOperatorExpression>($1, std::move($2)); }
    | SIZEOF unary_expression           { $$ = std::make_unique<prefix::SizeofExpression>(std::move($2)); }
    | SIZEOF '(' type_name ')'          { $$ = std::make_unique<prefix::SizeofExpression>(std::move($3)); }
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
    | '(' type_name ')' cast_expression { $$ = std::make_unique<CastExpression>(std::move($2), std::move($4)); }
    ;

multiplicative_expression
    : cast_expression                               { $$ = std::move($1); }
    | multiplicative_expression '*' cast_expression { $$ = std::make_unique<binary::AddMultExpression>(std::move($1), std::move($3), binary::AddMultExpressionType::MUL); }
    | multiplicative_expression '/' cast_expression { $$ = std::make_unique<binary::AddMultExpression>(std::move($1), std::move($3), binary::AddMultExpressionType::DIV); }
    | multiplicative_expression '%' cast_expression { $$ = std::make_unique<binary::AddMultExpression>(std::move($1), std::move($3), binary::AddMultExpressionType::REM); }
    ;

additive_expression
    : multiplicative_expression                         { $$ = std::move($1); }
    | additive_expression '+' multiplicative_expression { $$ = std::make_unique<binary::AddMultExpression>(std::move($1), std::move($3), binary::AddMultExpressionType::ADD); }
    | additive_expression '-' multiplicative_expression { $$ = std::make_unique<binary::AddMultExpression>(std::move($1), std::move($3), binary::AddMultExpressionType::SUB); }
    ;

shift_expression
    : additive_expression                           { $$ = std::move($1); }
    | shift_expression LEFT_OP additive_expression  { $$ = std::make_unique<binary::ShiftExpression>(std::move($1), std::move($3)); }
    | shift_expression RIGHT_OP additive_expression { $$ = std::make_unique<binary::ShiftExpression>(std::move($1), std::move($3), true); }
    ;

relational_expression
    : shift_expression                              { $$ = std::move($1); }
    | relational_expression '<' shift_expression    { $$ = std::make_unique<binary::RelationalExpression>(std::move($1), std::move($3), binary::RelationalExpressionType::L); }
    | relational_expression '>' shift_expression    { $$ = std::make_unique<binary::RelationalExpression>(std::move($1), std::move($3), binary::RelationalExpressionType::G); }
    | relational_expression LE_OP shift_expression  { $$ = std::make_unique<binary::RelationalExpression>(std::move($1), std::move($3), binary::RelationalExpressionType::LE); }
    | relational_expression GE_OP shift_expression  { $$ = std::make_unique<binary::RelationalExpression>(std::move($1), std::move($3), binary::RelationalExpressionType::GE); }
    ;

equality_expression
    : relational_expression                             { $$ = std::move($1); }
    | equality_expression EQ_OP relational_expression   { $$ = std::make_unique<binary::EqualityExpression>(std::move($1), std::move($3)); }
    | equality_expression NE_OP relational_expression   { $$ = std::make_unique<binary::EqualityExpression>(std::move($1), std::move($3), true); }
    ;

and_expression
    : equality_expression                       { $$ = std::move($1); }
    | and_expression '&' equality_expression    { $$ = std::make_unique<binary::BitwiseExpression>(std::move($1), std::move($3), binary::BitwiseExpressionType::AND); }
    ;

exclusive_or_expression
    : and_expression                                { $$ = std::move($1); }
    | exclusive_or_expression '^' and_expression    { $$ = std::make_unique<binary::BitwiseExpression>(std::move($1), std::move($3), binary::BitwiseExpressionType::XOR); }
    ;

inclusive_or_expression
    : exclusive_or_expression                               { $$ = std::move($1); }
    | inclusive_or_expression '|' exclusive_or_expression   { $$ = std::make_unique<binary::BitwiseExpression>(std::move($1), std::move($3), binary::BitwiseExpressionType::OR); }
    ;

logical_and_expression
    : inclusive_or_expression                               { $$ = std::move($1); }
    | logical_and_expression AND_OP inclusive_or_expression { $$ = std::make_unique<binary::LogicalExpression>(std::move($1), std::move($3), binary::LogicalExpressionType::AND); }
    ;

logical_or_expression
    : logical_and_expression                                { $$ = std::move($1); }
    | logical_or_expression OR_OP logical_and_expression    { $$ = std::make_unique<binary::LogicalExpression>(std::move($1), std::move($3), binary::LogicalExpressionType::OR); }
    ;

conditional_expression
    : logical_or_expression                                             { $$ = std::move($1); }
    | logical_or_expression '?' expression ':' conditional_expression   { $$ = std::make_unique<ConditionalExpression>(std::move($1), std::move($3), std::move($5))}
    ;

assignment_expression
    : conditional_expression                                        { $$ = std::move($1); }
    | unary_expression assignment_operator assignment_expression    { $$ = std::make_unique<binary::AssignmentExpression>(std::move($1), std::move($3), $2); }
    ;

assignment_operator
    : '='           { $$ = binary::AssignmentExpressionType::ASSIGN; }
    | MUL_ASSIGN    { $$ = binary::AssignmentExpressionType::MUL_ASSIGN; }
    | DIV_ASSIGN    { $$ = binary::AssignmentExpressionType::DIV_ASSIGN; }
    | MOD_ASSIGN    { $$ = binary::AssignmentExpressionType::MOD_ASSIGN; }
    | ADD_ASSIGN    { $$ = binary::AssignmentExpressionType::ADD_ASSIGN; }
    | SUB_ASSIGN    { $$ = binary::AssignmentExpressionType::SUB_ASSIGN; }
    | LEFT_ASSIGN   { $$ = binary::AssignmentExpressionType::LEFT_ASSIGN; }
    | RIGHT_ASSIGN  { $$ = binary::AssignmentExpressionType::RIGHT_ASSIGN; }
    | AND_ASSIGN    { $$ = binary::AssignmentExpressionType::AND_ASSIGN; }
    | XOR_ASSIGN    { $$ = binary::AssignmentExpressionType::XOR_ASSIGN; }
    | OR_ASSIGN     { $$ = binary::AssignmentExpressionType::OR_ASSIGN; }
    ;

expression
    : assignment_expression                 { $$ = std::make_unique<ListExpression>(std::move($1)); }
    | expression ',' assignment_expression  { $1->pushBack(std::move($3)); $$ = std::move($1); }
    ;

constant_expression
    : conditional_expression    { $$ = std::make_unique<ConstantExpression>(std::move($1)); } // Cannot be flattened, as evaluated at compile time.
    ;

declaration
    : declaration_specifiers ';'
    | declaration_specifiers init_declarator_list ';'
    ;

declaration_specifiers
    : storage_class_specifier                           { $$ = std::make_unique<DeclarationSpecifiers>(std::move($1), nullptr); }
    | storage_class_specifier declaration_specifiers    { $2->pushBackStorage(std::move($1)); $$ = std::move($2); }
    | type_specifier                                    { $$ = std::make_unique<DeclarationSpecifiers>(nullptr, std::move($1)); }
    | type_specifier declaration_specifiers             { $2->pushBackType(std::move($1)); $$ = std::move($2); }
    ;

init_declarator_list
    : init_declarator                           { $$ = std::make_unique<NodeList<DeclaratorBase>>(std::move($1)); }
    | init_declarator_list ',' init_declarator  { $1->pushBack(std::move($3)); $$ = std::move($1); }
    ;

init_declarator
    : declarator                    { $$ = std::move($1); }
    | declarator '=' initializer    { $$ = std::make_unique<InitDeclarator>(std::move($1), std::move($3))}
    ;

storage_class_specifier
    : TYPEDEF           { $$ = std::make_unique<ValueNode<StorageClassSpecifier>>(StorageClassSpecifier::TYPEDEF); }
    | EXTERN            { $$ = std::make_unique<ValueNode<StorageClassSpecifier>>(StorageClassSpecifier::EXTERN); }
    | STATIC            { $$ = std::make_unique<ValueNode<StorageClassSpecifier>>(StorageClassSpecifier::STATIC); }
    | AUTO              { $$ = std::make_unique<ValueNode<StorageClassSpecifier>>(StorageClassSpecifier::AUTO); }
    | REGISTER          { $$ = std::make_unique<ValueNode<StorageClassSpecifier>>(StorageClassSpecifier::REGISTER); }
    ;

type_specifier
    : VOID              { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::VOID); }
    | CHAR              { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::CHAR); }
    | SHORT             { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::SHORT); }
    | INT               { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::INT); }
    | LONG              { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::LONG); }
    | FLOAT             { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::FLOAT); }
    | DOUBLE            { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::DOUBLE); }
    | SIGNED            { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::SIGNED); }
    | UNSIGNED          { $$ = std::make_unique<ValueNode<TypeSpecifier>>(TypeSpecifier::UNSIGNED); }
    | struct_specifier  { $$ = std::move($1); }
    | enum_specifier    { $$ = std::move($1); }
    | TYPE_NAME         { $$ = std::make_unique<ValueNode<std::string>>($1); }
    ;

struct_specifier
    : STRUCT IDENTIFIER '{' struct_declaration_list '}' { $$ = std::make_unique<StructSpecifier>(std::move($4), $2); }
    | STRUCT '{' struct_declaration_list '}'            { $$ = std::make_unique<StructSpecifier>(std::move($3)); }
    | STRUCT IDENTIFIER                                 { $$ = std::make_unique<StructSpecifier>(nullptr, $2); }
    ;

struct_declaration_list
    : struct_declaration                            { $$ = std::make_unique<NodeList<StructDeclaration>>(std::move($1)); }
    | struct_declaration_list struct_declaration    { $1->pushBack(std::move($2)); $$ = std::move($1); }
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'   { $$ = std::make_unique<StructDeclaration>(std::move($1), std::move($2)); }
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
    : assignment_expression         { $$ = std::move($1); }
    | '{' initializer_list '}'      { $$ = std::make_unique<Initializer>(std::move($2)); }
    | '{' initializer_list ',' '}'  { $$ = std::make_unique<Initializer>(std::move($2)); }
    ;

initializer_list
    : initializer                       { $$ = std::make_unique<NodeList<ExpressionBase>>(std::move($1)); }
    | initializer_list ',' initializer  { $1->pushBack(std::move($3)); $$ = std::move($1); }
    ;

statement
    : labelled_statement    { $$ = std::move($1); }
    | compound_statement    { $$ = std::move($1); }
    | expression_statement  { $$ = std::move($1); }
    | selection_statement   { $$ = std::move($1); }
    | iteration_statement   { $$ = std::move($1); }
    | jump_statement        { $$ = std::move($1); }
    ;

labelled_statement
    : IDENTIFIER ':' statement                  { $$ = std::make_unique<LabelledStatement>($1, std::move($3)); }
    | CASE constant_expression ':' statement    { $$ = std::make_unique<SwitchCaseStatement>(std::move($2), std::move($4)); }
    | DEFAULT ':' statement                     { $$ = std::make_unique<SwitchCaseStatement>(std::move($3)); }
    ;

compound_statement
    : '{' { typedef_table.pushScope(); } declaration_list_opt statement_list_opt '}'    { typedef_table.popScope(); $$ = std::make_unique<CompoundStatement>(std::move($3), std::move($4)); }
    ;

declaration_list
    : declaration                   { $$ = std::make_unique<NodeList<DeclarationBase>>(std::move($1)); }
    | declaration_list declaration  { $1->pushBack(std::move($2)); $$ = std::move($1); }
    ;

declaration_list_opt
    : %empty            { $$ = nullptr; }
    | declaration_list  { $$ = std::move($1); }
    ;

statement_list
    : statement                 { $$ = std::make_unique<NodeList<StatementBase>>(std::move($1)); }
    | statement_list statement  { $1->pushBack(std::move($2)); $$ = std::move($1); }
    ;

statement_list_opt
    : %empty            { $$ = nullptr; }
    | statement_list    { $$ = std::move($1); }
    ;

expression_statement
    : ';'                       { $$ = std::make_unique<ExpressionStatement>(); }
    | expression ';'            { $$ = std::make_unique<ExpressionStatement>(std::move($1)); }
    ;

selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE { $$ = std::make_unique<IfStatement>(std::move($3), std::move($5)); }
    | IF '(' expression ')' statement ELSE statement        { $$ = std::make_unique<IfStatement>(std::move($3), std::move($5), std::move($7)); }
    | SWITCH '(' expression ')' statement                   { $$ = std::make_unique<SwitchStatement>(std::move($3), std::move($5)); }
    ;

iteration_statement
    : WHILE '(' expression ')' statement                                            { $$ = IterationStatement::While(std::move($3), std::move($5)); }
    | DO statement WHILE '(' expression ')' ';'                                     { $$ = IterationStatement::DoWhile(std::move($5), std::move($2)); }
    | FOR '(' expression_statement expression_statement ')' statement               { $$ = IterationStatement::For(/* TODO what? */); }
    | FOR '(' expression_statement expression_statement expression ')' statement    { $$ = IterationStatement::For(std::move($3), std::move($4), std::move($5), std::move($7)); }
    ;

jump_statement
    : GOTO IDENTIFIER ';'   { $$ = std::make_unqiue<GotoStatement>($2); }
    | CONTINUE ';'          { $$ = std::make_unique<LoopControlStatement>(false); }
    | BREAK ';'             { $$ = std::make_unique<LoopControlStatement>(true); }
    | RETURN ';'            { $$ = std::make_unique<ReturnStatement>(); }
    | RETURN expression ';' { $$ = std::make_unique<ReturnStatement>(NodePtr($2)); }
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
