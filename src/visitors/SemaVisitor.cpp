#include "visitors/SemaVisitor.h"

#include <cassert>
#include <cstddef>
#include <format>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "ast/Node.h"
#include "ast/StorageClassSpecifier.h"
#include "ast/TypeQualifier.h"
#include "ast/TypeSpecifier.h"
#include "ast/all.h"
#include "ast/expressions/ListExpression.h"
#include "errors/errors.h"
#include "types/Scope.h"
#include "types/SymbolTable.h"
#include "types/Type.h"
#include "utils.h"
#include "visitors/RecursiveVisitor.h"

namespace thogcc::visitors {

std::shared_ptr<types::Type> SemaVisitor::getPromotedType(const std::shared_ptr<types::Type>& lhs,
                                                          const std::shared_ptr<types::Type>& rhs) {
    const auto* lBasic = std::get_if<types::BasicType>(&lhs->data);
    const auto* rBasic = std::get_if<types::BasicType>(&rhs->data);

    if ((lBasic == nullptr) || (rBasic == nullptr)) {
        throw errors::SemaError("Uimplemented: implicit promotion to non-basic type");
    }

    auto getRank = [](types::BasicType::Kind k) {
        switch (k) {
            case types::BasicType::Kind::DOUBLE:
                return 6;
            case types::BasicType::Kind::FLOAT:
                return 5;
            case types::BasicType::Kind::LONG:
                return 4;
            case types::BasicType::Kind::INT:
                return 3;
            case types::BasicType::Kind::SHORT:
                return 2;
            case types::BasicType::Kind::CHAR:
                return 1;
            default:
                return -1;
        }
    };

    // TODO compare .isUnsigned

    if (getRank(lBasic->kind) >= getRank(rBasic->kind)) {
        return lhs;
    }
    return rhs;
}

void SemaVisitor::visitVal(ast::ValueNode<ast::TypeSpecifier>& valNode) {
    types::BasicType basicType;
    if (auto* b = std::get_if<types::BasicType>(&_curType.data)) {
        basicType = *b;
    }

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
            break;
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

void SemaVisitor::visit(ast::declarations::FunctionDefinition& node) {
    _curType = types::Type{};

    _table.pushScope();

    node.getSpecifiers()->accept(*this);
    node.getDeclarator()->accept(*this);

    std::string funcName{node.getDeclarator()->getIdentifier()};

    if (node.getDeclarations() != nullptr) {
        throw errors::SemaError("K&R FunctionDefinition not supported.");
    }

    // Must do this here to avoid CompountStatement pushing an extra scope
    if (node.getStatement()->getDeclarationList() != nullptr) {
        node.getStatement()->getDeclarationList()->accept(*this);
    }

    if (node.getStatement()->getStatementList() != nullptr) {
        node.getStatement()->getStatementList()->accept(*this);
    }

    _table.popScope();

    auto ordSymb = _table.getOrd(funcName);
    auto funcSymb = std::get<std::shared_ptr<types::FuncSymbol>>(ordSymb);
    node.setSymbol(funcSymb);
}

void SemaVisitor::visit(ast::declarations::ParameterDeclaration& node) {
    _curType = types::Type{};
    node.getSpecifiers()->accept(*this);

    if (node.getDecl() != nullptr) node.getDecl()->accept(*this);

    // TODO holds_alternative<ArrayType>

    auto varSymb = types::VarSymbol{std::make_shared<types::Type>(_curType)};
    auto sharedSymb = std::make_shared<types::VarSymbol>(varSymb);
    node.setSymbol(sharedSymb);
    // TODO add to _table here or elsewhere?
}

void SemaVisitor::visit(ast::declarators::FunctionDeclarator& node) {
    auto returnType = std::make_shared<types::Type>(_curType);

    if (node.getForm() == ast::declarators::FunctionDeclaratorForm::KAndR) {
        throw errors::SemaError("Unimplemented: K&R");
    }

    std::vector<std::shared_ptr<types::Type>> params;
    if (node.getParams() != nullptr) {
        for (const auto& p : node.getParams()->getNodes()) {
            p->accept(*this);
            params.push_back(std::make_shared<types::Type>(_curType));
        }
    }

    types::FuncType funcType{returnType, params};
    auto sharedSymb = std::make_shared<types::FuncSymbol>(std::make_shared<types::Type>(funcType),
                                                          true  // TODO fix
    );

    // Do not call node.getBase()->accept(*this) I think?
    _table.addToParentScope(std::string{node.getIdentifier()}, {sharedSymb});
}

void SemaVisitor::visit(ast::declarators::IdentifierDeclarator& node) {
    types::VarSymbol varSymb{std::make_shared<types::Type>(_curType)};
    auto sharedSymb = std::make_shared<types::VarSymbol>(varSymb);
    _table.addToScope(std::string{node.getIdentifier()}, {sharedSymb});
    node.setSymbol(sharedSymb);
}

void SemaVisitor::visit(ast::declarators::InitDeclarator& node) {
    RecursiveVisitor::visit(node);
    auto name = node.getIdentifier();
    auto symb = _table.getOrd(std::string{name});
    node.setSymbol(symb);
}

void SemaVisitor::visit(ast::declarators::PointerDeclarator& node) {
    auto ptrType = types::Type{};
    ptrType.data = types::PointerType{std::make_shared<types::Type>(_curType)};

    auto baseType = _curType;
    _curType = ptrType;

    RecursiveVisitor::visit(node);

    _curType = baseType;
}

void SemaVisitor::visit(ast::expressions::IdentifierExpression& node) {
    auto symb = _table.getOrd(std::string{node.getIdentifier()});
    auto resolvedType = std::visit([](auto& s) { return s.get()->type; }, symb);
    node.setSymbol(symb);
    node.setEvaluatedType(resolvedType);
    node.setIsLvalue(true);  // should always be an lvalue I think?
}

void SemaVisitor::visit(ast::expressions::ListExpression& node) {
    std::shared_ptr<types::Type> lastType = nullptr;

    for (const auto& expr : node.getList()->getNodes()) {
        expr->accept(*this);
        lastType = expr->getEvaluatedType();
    }

    if (lastType == nullptr) {
        throw errors::SemaError("ListExpression return no lastType.");
    }

    node.setEvaluatedType(lastType);
    node.setIsLvalue(false);  // comma operator should always yield an rvalue?
}

void SemaVisitor::visit(ast::expressions::PrimaryExpression& node) {
    auto type = std::make_shared<types::Type>();
    std::visit(overload{
                   [&type](const int& /* x */) {
                       type->data = types::BasicType{types::BasicType::Kind::INT};
                   },
                   [&type](const double& /* x */) {
                       type->data = types::BasicType{types::BasicType::Kind::DOUBLE};
                   },
                   [](const auto& /* x */) { assert(false && "TODO: strings"); },
               },
               node.getValue());
    node.setEvaluatedType(type);
    node.setIsLvalue(false);
}

void SemaVisitor::visit(ast::expressions::binary::AddMultExpression& node) {
    node.getLhs()->accept(*this);
    auto lhsType = node.getLhs()->getEvaluatedType();

    node.getRhs()->accept(*this);
    auto rhsType = node.getRhs()->getEvaluatedType();

    auto resultType = getPromotedType(lhsType, rhsType);

    node.setEvaluatedType(resultType);
    node.setIsLvalue(false);
}

void SemaVisitor::visit(ast::expressions::binary::AssignmentExpression& node) {
    node.getLhs()->accept(*this);
    auto lhsType = node.getLhs()->getEvaluatedType();
    const bool lhsIsLval = node.getLhs()->isLvalue();

    node.getRhs()->accept(*this);
    auto rhsType = node.getRhs()->getEvaluatedType();

    if (!lhsIsLval) {
        throw errors::SemaError("AssignmentExpression LHS is not lvalue");
    }

    if (lhsType->isConst) {
        throw errors::SemaError("Assignment to constant variable");
    }

    // TODO implicit conversion and type checking lhs = rhs

    node.setEvaluatedType(lhsType);
    node.setIsLvalue(false);
}

void SemaVisitor::visit(ast::expressions::binary::EqualityExpression& node) {
    node.getLhs()->accept(*this);
    auto lhsType = node.getLhs()->getEvaluatedType();

    node.getRhs()->accept(*this);
    auto rhsType = node.getRhs()->getEvaluatedType();

    auto resultType = std::make_shared<types::Type>(types::BasicType{types::BasicType::Kind::INT});

    node.setEvaluatedType(resultType);
    node.setIsLvalue(false);
}

void SemaVisitor::visit(ast::expressions::postfix::FunctionCallExpression& node) {
    node.getExpr()->accept(*this);
    auto calleeType = node.getExpr()->getEvaluatedType();
    auto* funcType = std::get_if<types::FuncType>(&calleeType->data);
    if (funcType == nullptr) {
        throw errors::SemaError("Expected a FuncType");
    }

    size_t funcArgs = 0;
    if (node.getArgs() != nullptr) {
        funcArgs = node.getArgs()->size();
        node.getArgs()->accept(*this);
    }

    if ((funcArgs != funcType->params.size())) {
        throw errors::SemaError(std::format("Function expected {} arguments, got {}",
                                            funcType->params.size(), node.getArgs()->size()));
    }

    node.setEvaluatedType(funcType->returnType);
    node.setIsLvalue(false);
}

void SemaVisitor::visit(ast::statements::CompoundStatement& node) {
    _table.pushScope();
    RecursiveVisitor::visit(node);
    _table.popScope();
}

}  // namespace thogcc::visitors
