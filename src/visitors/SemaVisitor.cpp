#include "visitors/SemaVisitor.h"

#include "ast/Node.h"
#include "ast/StorageClassSpecifier.h"
#include "ast/TypeQualifier.h"
#include "ast/TypeSpecifier.h"
#include "ast/all.h"
#include "ast/declarators/IdentifierDeclarator.h"
#include "types/SymbolTable.h"
#include "types/Type.h"
#include "visitors/RecursiveVisitor.h"

namespace thogcc::visitors {

void SemaVisitor::visitVal(ast::ValueNode<ast::TypeSpecifier>& valNode) {
    types::BasicType basicType;

    switch (valNode.getValue()) {
        case ast::TypeSpecifier::VOID:
            basicType.kind = types::BasicType::Kind::VOID;
            break;
        case ast::TypeSpecifier::CHAR:
            basicType.kind = types::BasicType::Kind::CHAR;
            break;
        case ast::TypeSpecifier::SHORT:
            basicType.kind = types::BasicType::Kind::SHORT;
            break;
        case ast::TypeSpecifier::INT:
            basicType.kind = types::BasicType::Kind::INT;
            break;
        case ast::TypeSpecifier::LONG:
            basicType.kind = types::BasicType::Kind::LONG;
            break;
        case ast::TypeSpecifier::FLOAT:
            basicType.kind = types::BasicType::Kind::FLOAT;
            break;
        case ast::TypeSpecifier::DOUBLE:
            basicType.kind = types::BasicType::Kind::DOUBLE;
            break;
        case ast::TypeSpecifier::SIGNED:
            basicType.isUnsigned = false;
            break;
        case ast::TypeSpecifier::UNSIGNED:
            basicType.isUnsigned = true;
        case ast::TypeSpecifier::STRUCT:
        case ast::TypeSpecifier::ENUM:
        case ast::TypeSpecifier::TYPE_NAME:
            assert(false && "TODO");
            // TODO tagged types and TYPE_NAME
            break;
    }

    _curType.data = basicType;
}

void SemaVisitor::visitVal(ast::ValueNode<ast::StorageClassSpecifier>& valNode) {
    switch (valNode.getValue()) {
        case ast::StorageClassSpecifier::TYPEDEF:
        case ast::StorageClassSpecifier::EXTERN:
        case ast::StorageClassSpecifier::STATIC:
        case ast::StorageClassSpecifier::AUTO:
        case ast::StorageClassSpecifier::REGISTER:
            // TODO
            break;
    }
}

void SemaVisitor::visitVal(ast::ValueNode<ast::TypeQualifier>& valNode) {
    switch (valNode.getValue()) {
        case ast::TypeQualifier::CONST:
            _curType.isConst = true;
            break;
        case ast::TypeQualifier::VOLATILE:
            _curType.isVolatile = true;
            break;
    }
}

void SemaVisitor::visitVal(ast::ValueNode<std::string>& /*valNode*/) {}

void SemaVisitor::visit(ast::declarations::Declaration& node) {
    _curType = types::Type{};
    node.getSpecifiers()->accept(*this);
    node.getDeclarators()->accept(*this);
}

void SemaVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    types::OrdSymbol symbol{types::VarSymbol{std::make_shared<types::Type>(_curType)}};
    _table.addToScope(std::string{node.getIdentifier()}, symbol);
}

void SemaVisitor::visit(ast::expressions::IdentifierExpression& node) {
    auto symb = _table.getOrd(std::string{node.getIdentifier()});
    auto varSymb = std::get<types::VarSymbol>(symb);
    node.setSymbol(varSymb);
}

void SemaVisitor::visit(ast::statements::CompoundStatement& node) {
    _table.pushScope();
    RecursiveVisitor::visit(node);
    _table.popScope();
}

}  // namespace thogcc::visitors
