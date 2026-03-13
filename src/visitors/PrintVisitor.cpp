#include "visitors/PrintVisitor.h"

#include <cassert>
#include <cstddef>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

#include "ast/Node.h"
#include "ast/all.h"
#include "ast/utils.h"

namespace thogcc::visitors {

template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

PrintVisitor::PrintVisitor(std::ostream& out) : _out(out) {
    _out << "%%{init: {\"flowchart\": {\"defaultRenderer\": \"elk\"}} }%%\n";
    _out << "flowchart TD\n";
};

void PrintVisitor::printNode(int id, ast::Node& node) {
    _out << std::format("  n{}[{}]\n", id, ast::nodeKindName(node.getKind()));
}

template <typename T>
void PrintVisitor::visitChild(int cur, std::string_view label, T* child) {
    if (!child) return;

    _out << std::format("  n{} -->|{}| n{}\n", cur, label, ++_id);
    child->accept(*this);
}

// Base classes

void PrintVisitor::visit(ast::Node& node) {
    assert(false);
    _out << std::format("  n{}[Unimplemented]\n", _id)
         << std::format("%%Unimplemented: {}\n", ast::nodeKindName(node.getKind()));
};

void PrintVisitor::visit(ast::declarations::DeclarationBase& node) {
    visit(static_cast<ast::Node&>(node));
};

void PrintVisitor::visit(ast::declarators::DeclaratorBase& node) {
    visit(static_cast<ast::Node&>(node));
};

void PrintVisitor::visit(ast::expressions::ExpressionBase& node) {
    visit(static_cast<ast::Node&>(node));
};

void PrintVisitor::visit(ast::statements::StatementBase& node) {
    visit(static_cast<ast::Node&>(node));
};

// Special nodes

void PrintVisitor::visit(ast::NodeListBase& node) {
    const int cur = _id;
    _out << std::format("  n{}[/NodeList/]\n", cur);
    for (size_t i = 0; i < node.size(); ++i) {
        _out << std::format("  n{} --> n{}\n", cur, ++_id);
        node.getRawNode(i).accept(*this);  // Recursively visit each element
    }
};

void PrintVisitor::visit(ast::ValueNodeBase& node) {
    _out << std::format("  n{}[{}]\n", _id, node.getLabel());
};

// Leaf nodes

void PrintVisitor::visit(ast::DeclarationSpecifiers& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "StorageClassSpecifier", node.getStorageClassSpecifier());
    visitChild(cur, "TypeSpecifier", node.getTypeSpecifiers());
}

void PrintVisitor::visit(ast::declarations::Declaration& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Specifiers", node.getSpecifiers());
    visitChild(cur, "Declarators", node.getDeclarators());
}

void PrintVisitor::visit(ast::declarations::FunctionDefinition& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Specifiers", node.getSpecifiers());
    visitChild(cur, "Declarator", node.getDeclarator());
    visitChild(cur, "Declarations", node.getDeclarations());
    visitChild(cur, "Statement", node.getStatement());
};

void PrintVisitor::visit(ast::declarations::ParameterDeclaration& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Specifiers", node.getSpecifiers());
    visitChild(cur, "Declarator", node.getDecl());
}

void PrintVisitor::visit(ast::declarations::StructDeclaration& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Qualifiers", node.getQualifiers());
    visitChild(cur, "Declarators", node.getDeclarators());
}

void PrintVisitor::visit(ast::declarators::ArrayDeclarator& node) {
    const int cur = _id;
    printNode(cur, node);

    visitChild(cur, "DeclaratorBase", node.getBase());
    visitChild(cur, "Expression", node.getExpr());
}

void PrintVisitor::visit(ast::declarators::EnumValueDeclarator& node) {
    const int cur = _id;
    printNode(cur, node);
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
    visitChild(cur, "ConstExpr", node.getExpr());
}

void PrintVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    const int cur = _id;
    auto form = node.getForm();
    _out << std::format(
        "  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()),
        form == ast::declarators::FunctionDeclaratorForm::KAndR ? "K&R" : "prototype");
    visitChild(cur, "Base", node.getBase());
    visitChild(cur, "Identifiers", node.getIdentifiers());
    visitChild(cur, "Params", node.getParams());
};

void PrintVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    const int cur = _id;
    printNode(cur, node);
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
};

void PrintVisitor::visit(ast::declarators::InitDeclarator& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Declarator", node.getDecl());
    visitChild(cur, "Initializer", node.getInitializer());
}

void PrintVisitor::visit(ast::declarators::PointerDeclarator& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "TypeQualifiers", node.getTypeQualifiers());
    visitChild(cur, "Pointer", node.getPtr());
}

void PrintVisitor::visit(ast::declarators::StructMemberDeclarator& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Declarator", node.getDecl());
    visitChild(cur, "Expr", node.getExpr());
}

void PrintVisitor::visit(ast::expressions::CastExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "CastType", node.getTypeName());
    visitChild(cur, "Expr", node.getExpr());
}

void PrintVisitor::visit(ast::expressions::ConditionalExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Condition", node.getCond());
    visitChild(cur, "If", node.getIfExpr());
    visitChild(cur, "Else", node.getElseExpr());
}

void PrintVisitor::visit(ast::expressions::ConstantExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Expr", node.getExpr());
}

void PrintVisitor::visit(ast::expressions::IdentifierExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
};

void PrintVisitor::visit(ast::expressions::IncDecExpression& node) {
    const int cur = _id;
    _out << std::format("  n{}[\"{} ({} {})\"]\n", cur, ast::nodeKindName(node.getKind()),
                        node.getIsPrefix() ? "Prefix" : "Postfix",
                        node.getIsDecrement() ? "Dec" : "Inc");
    visitChild(cur, "Expr", node.getExpr());
}

void PrintVisitor::visit(ast::expressions::ListExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "List", node.getList());
};

void PrintVisitor::visit(ast::expressions::PrimaryExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    const auto value = node.getValue();
    std::visit(
        [this, cur](auto&& v) {
            _out << std::format("  n{} -->|Value| n{}([\"{}{}\"])\n", cur, ++_id, v,
                                typeid(v).name());
        },
        value);
};

void PrintVisitor::visit(ast::expressions::binary::BinaryExpressionBase& node) {
    const int cur = _id;
    visitChild(cur, "LHS", node.getLhs());
    visitChild(cur, "RHS", node.getRhs());
};

void PrintVisitor::visit(ast::expressions::binary::AddMultExpression& node) {
    const int cur = _id;
    char op = '?';
    switch (node.getOp()) {
        case ast::expressions::binary::AddMultExpressionType::ADD:
            op = '+';
            break;
        case ast::expressions::binary::AddMultExpressionType::SUB:
            op = '-';
            break;
        case ast::expressions::binary::AddMultExpressionType::MUL:
            op = '*';
            break;
        case ast::expressions::binary::AddMultExpressionType::DIV:
            op = '/';
            break;
        case ast::expressions::binary::AddMultExpressionType::REM:
            op = '%';
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), op);
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    const int cur = _id;
    std::string op = "?";
    switch (node.getOp()) {
        case ast::expressions::binary::AssignmentExpressionType::ASSIGN:
            op = "=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::MUL_ASSIGN:
            op = "*=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::DIV_ASSIGN:
            op = "/=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::MOD_ASSIGN:
            op = "%=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::ADD_ASSIGN:
            op = "+=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::SUB_ASSIGN:
            op = "-=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::LEFT_ASSIGN:
            op = "<<=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::RIGHT_ASSIGN:
            op = ">>=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::AND_ASSIGN:
            op = "&=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::XOR_ASSIGN:
            op = "^=";
            break;
        case ast::expressions::binary::AssignmentExpressionType::OR_ASSIGN:
            op = "|=";
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), op);
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::binary::BitwiseExpression& node) {
    const int cur = _id;
    char op = '?';
    switch (node.getOp()) {
        case ast::expressions::binary::BitwiseExpressionType::AND:
            op = '&';
            break;
        case ast::expressions::binary::BitwiseExpressionType::OR:
            op = '|';
            break;
        case ast::expressions::binary::BitwiseExpressionType::XOR:
            op = '^';
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), op);
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::binary::EqualityExpression& node) {
    const int cur = _id;
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()),
                        node.getIsNe() ? "!=" : "==");
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::binary::LogicalExpression& node) {
    const int cur = _id;
    std::string op = "??";
    switch (node.getOp()) {
        case ast::expressions::binary::LogicalExpressionType::AND:
            op = "&&";
            break;
        case ast::expressions::binary::LogicalExpressionType::OR:
            op = "||";
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), op);
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::binary::RelationalExpression& node) {
    const int cur = _id;
    std::string op = "??";
    switch (node.getOp()) {
        case ast::expressions::binary::RelationalExpressionType::L:
            op = "<";
            break;
        case ast::expressions::binary::RelationalExpressionType::G:
            op = ">";
            break;
        case ast::expressions::binary::RelationalExpressionType::LE:
            op = "<=";
            break;
        case ast::expressions::binary::RelationalExpressionType::GE:
            op = ">=";
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), op);
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::binary::ShiftExpression& node) {
    const int cur = _id;
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()),
                        node.getIsRightShift() ? ">>" : "<<");
    visit(static_cast<ast::expressions::binary::BinaryExpressionBase&>(node));
}

void PrintVisitor::visit(ast::expressions::postfix::ArrayAccessExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Array", node.getArray());
    visitChild(cur, "Index", node.getIndex());
}

void PrintVisitor::visit(ast::expressions::postfix::FunctionCallExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Function", node.getExpr());
    visitChild(cur, "Args", node.getArgs());
}

void PrintVisitor::visit(ast::expressions::postfix::MemberAccessExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Parent", node.getExpr());
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
}

void PrintVisitor::visit(ast::expressions::prefix::SizeofExpression& node) {
    const int cur = _id;
    printNode(cur, node);
    std::visit([this, cur](const auto& ptr) { visitChild(cur, "Expr", ptr.get()); },
               node.getExpr());
};

void PrintVisitor::visit(ast::expressions::prefix::UnaryOperatorExpression& node) {
    const int cur = _id;
    char symbol = '?';
    switch (node.getType()) {
        case ast::expressions::prefix::UnaryOperatorType::ADDRESSOF:
            symbol = '&';
            break;
        case ast::expressions::prefix::UnaryOperatorType::INDIRECTION:
            symbol = '*';
            break;
        case ast::expressions::prefix::UnaryOperatorType::PLUS:
            symbol = '+';
            break;
        case ast::expressions::prefix::UnaryOperatorType::MINUS:
            symbol = '-';
            break;
        case ast::expressions::prefix::UnaryOperatorType::BITWISE_NOT:
            symbol = '~';
            break;
        case ast::expressions::prefix::UnaryOperatorType::LOGICAL_NOT:
            symbol = '!';
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), symbol);
    visitChild(cur, "Expr", node.getExpr());
}

void PrintVisitor::visit(ast::expressions::Initializer& node) {
    const int cur = _id;
    printNode(cur, node);

    std::visit(
        overload{
            [this, cur](const std::unique_ptr<ast::expressions::ExpressionBase>& e) {
                visitChild(cur, "Expression", e.get());
            },
            [this, cur](const std::unique_ptr<ast::NodeList<ast::expressions::Initializer>>& e) {
                visitChild(cur, "InitializerList", e.get());
            },
        },
        node.getChild());
}

void PrintVisitor::visit(ast::statements::CompoundStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "StatementList", node.getStatementList());
    visitChild(cur, "DeclarationList", node.getDeclarationList());
};

void PrintVisitor::visit(ast::statements::ExpressionStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Expr", node.getExpr());
}

void PrintVisitor::visit(ast::statements::GotoStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
}

void PrintVisitor::visit(ast::statements::IfStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Cond", node.getCond());
    visitChild(cur, "If", node.getIfStatement());
    visitChild(cur, "Else", node.getElseStatement());
}

void PrintVisitor::visit(ast::statements::IterationStatement& node) {
    const int cur = _id;
    std::string type = "??";
    switch (node.getType()) {
        case ast::statements::IterationStatementType::WHILE:
            type = "while";
            break;
        case ast::statements::IterationStatementType::DOWHILE:
            type = "do while";
            break;
        case ast::statements::IterationStatementType::FOR:
            type = "for";
            break;
    }
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()), type);
    visitChild(cur, "Statement", node.getStatement());
    visitChild(cur, "InitExpr", node.getInitExpr());
    visitChild(cur, "CondExpr", node.getCondExpr());
    visitChild(cur, "UpdateExpr", node.getUpdateExpr());
}

void PrintVisitor::visit(ast::statements::LabelledStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
    visitChild(cur, "Statement", node.getStatement());
}

void PrintVisitor::visit(ast::statements::LoopControlStatement& node) {
    const int cur = _id;
    _out << std::format("  n{}[\"{} ({})\"]\n", cur, ast::nodeKindName(node.getKind()),
                        node.getIsBreak() ? "break" : "continue");
}

void PrintVisitor::visit(ast::statements::SwitchCaseStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Cond", node.getCond());
    visitChild(cur, "Statement", node.getStatement());
}

void PrintVisitor::visit(ast::statements::SwitchStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "SwitchOn", node.getExpr());
    visitChild(cur, "Statement", node.getStatement());
}

void PrintVisitor::visit(ast::statements::ReturnStatement& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Expr", node.getExpr());
};

void PrintVisitor::visit(ast::EnumSpecifier& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Declarators", node.getDeclarators());
}

void PrintVisitor::visit(ast::StructSpecifier& node) {
    const int cur = _id;
    printNode(cur, node);
    _out << std::format("  n{} -->|Identifier| n{}([{}])\n", cur, ++_id, node.getIdentifier());
    visitChild(cur, "Declarations", node.getDeclarations());
}

void PrintVisitor::visit(ast::TypeName& node) {
    const int cur = _id;
    printNode(cur, node);
    visitChild(cur, "Specifiers", node.getSpecifiers());
    visitChild(cur, "Declarator", node.getDeclarator());
}

}  // namespace thogcc::visitors
