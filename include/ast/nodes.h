#pragma once

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define AST_NODES_ALL(V)                                 \
    /* Core things */                                    \
    V(ast, Node)                                         \
    V(ast, NodeListBase)                                 \
    V(ast, ValueNodeBase)                                \
    V(ast, DeclarationSpecifiers)                        \
    V(ast, EnumSpecifier)                                \
    V(ast, StructSpecifier)                              \
    V(ast, TypeName)                                     \
                                                         \
    /* declarations */                                   \
    V(ast::declarations, DeclarationBase)                \
    V(ast::declarations, Declaration)                    \
    V(ast::declarations, FunctionDefinition)             \
    V(ast::declarations, ParameterDeclaration)           \
    V(ast::declarations, StructDeclaration)              \
                                                         \
    /* declarators */                                    \
    V(ast::declarators, DeclaratorBase)                  \
    V(ast::declarators, ArrayDeclarator)                 \
    V(ast::declarators, EnumValueDeclarator)             \
    V(ast::declarators, FunctionDeclarator)              \
    V(ast::declarators, IdentifierDeclarator)            \
    V(ast::declarators, InitDeclarator)                  \
    V(ast::declarators, PointerDeclarator)               \
    V(ast::declarators, StructMemberDeclarator)          \
                                                         \
    /* expressions*/                                     \
    V(ast::expressions, ExpressionBase)                  \
    V(ast::expressions, CastExpression)                  \
    V(ast::expressions, ConditionalExpression)           \
    V(ast::expressions, ConstantExpression)              \
    V(ast::expressions, IdentifierExpression)            \
    V(ast::expressions, IncDecExpression)                \
    V(ast::expressions, Initializer)                     \
    V(ast::expressions, ListExpression)                  \
    V(ast::expressions, PrimaryExpression)               \
                                                         \
    /* expressions::binary */                            \
    V(ast::expressions::binary, BinaryExpressionBase)    \
    V(ast::expressions::binary, AddMultExpression)       \
    V(ast::expressions::binary, AssignmentExpression)    \
    V(ast::expressions::binary, BitwiseExpression)       \
    V(ast::expressions::binary, EqualityExpression)      \
    V(ast::expressions::binary, LogicalExpression)       \
    V(ast::expressions::binary, RelationalExpression)    \
    V(ast::expressions::binary, ShiftExpression)         \
                                                         \
    /* expressions::postfix */                           \
    V(ast::expressions::postfix, ArrayAccessExpression)  \
    V(ast::expressions::postfix, FunctionCallExpression) \
    V(ast::expressions::postfix, MemberAccessExpression) \
                                                         \
    /* expressions::prefix */                            \
    V(ast::expressions::prefix, SizeofExpression)        \
    V(ast::expressions::prefix, UnaryOperatorExpression) \
                                                         \
    /* statements */                                     \
    V(ast::statements, StatementBase)                    \
    V(ast::statements, CompoundStatement)                \
    V(ast::statements, ExpressionStatement)              \
    V(ast::statements, GotoStatement)                    \
    V(ast::statements, IfStatement)                      \
    V(ast::statements, IterationStatement)               \
    V(ast::statements, LabelledStatement)                \
    V(ast::statements, LoopControlStatement)             \
    V(ast::statements, ReturnStatement)                  \
    V(ast::statements, SwitchCaseStatement)              \
    V(ast::statements, SwitchStatement)
