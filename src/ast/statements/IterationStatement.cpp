#include "ast/statements/IterationStatement.h"

#include <memory>
#include <utility>

#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/ExpressionStatement.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

IterationStatement::IterationStatement(IterationStatementType type,
                                       std::unique_ptr<StatementBase> statement,
                                       std::unique_ptr<expressions::ExpressionBase> initExpr,
                                       std::unique_ptr<expressions::ExpressionBase> condExpr,
                                       std::unique_ptr<expressions::ExpressionBase> updateExpr)
    : _type(type),
      _statement(std::move(statement)),
      _initExpr(std::move(initExpr)),
      _condExpr(std::move(condExpr)),
      _updateExpr(std::move(updateExpr)){};

std::unique_ptr<IterationStatement> IterationStatement::While(
    std::unique_ptr<expressions::ExpressionBase> expr, std::unique_ptr<StatementBase> statement) {
    return std::make_unique<IterationStatement>(IterationStatementType::WHILE, std::move(statement),
                                                nullptr, std::move(expr), nullptr);
};

std::unique_ptr<IterationStatement> IterationStatement::DoWhile(
    std::unique_ptr<expressions::ExpressionBase> expr, std::unique_ptr<StatementBase> statement) {
    return std::make_unique<IterationStatement>(
        IterationStatementType::DOWHILE, std::move(statement), nullptr, std::move(expr), nullptr);
};

std::unique_ptr<IterationStatement> IterationStatement::For(
    std::unique_ptr<expressions::ExpressionBase> initExpr,
    std::unique_ptr<expressions::ExpressionBase> condExpr,
    std::unique_ptr<expressions::ExpressionBase> updateExpr,
    std::unique_ptr<StatementBase> statement) {
    return std::make_unique<IterationStatement>(IterationStatementType::FOR, std::move(statement),
                                                std::move(initExpr), std::move(condExpr),
                                                std::move(updateExpr));
};

}  // namespace thogcc::ast::statements
