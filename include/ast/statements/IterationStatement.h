#pragma once

#include <cstdint>
#include <memory>

#include "ast/Node.h"
#include "ast/expressions/ExpressionBase.h"
#include "ast/statements/ExpressionStatement.h"
#include "ast/statements/StatementBase.h"

namespace thogcc::ast::statements {

enum class IterationStatementType : std::uint8_t {
    WHILE,
    DOWHILE,
    FOR,
};

class IterationStatement : public VisitableNode<IterationStatement, StatementBase> {
   public:
    IterationStatement(IterationStatementType type, std::unique_ptr<StatementBase> statement,
                       std::unique_ptr<ExpressionStatement> initExpr = nullptr,
                       std::unique_ptr<ExpressionStatement> condExpr = nullptr,
                       std::unique_ptr<expressions::ExpressionBase> updateExpr = nullptr);

    static std::unique_ptr<IterationStatement> While(
        std::unique_ptr<expressions::ExpressionBase> expr,
        std::unique_ptr<StatementBase> statement);
    static std::unique_ptr<IterationStatement> DoWhile(
        std::unique_ptr<expressions::ExpressionBase> expr,
        std::unique_ptr<StatementBase> statement);
    static std::unique_ptr<IterationStatement> For(
        std::unique_ptr<ExpressionStatement> initExpr,
        std::unique_ptr<ExpressionStatement> condExpr,
        std::unique_ptr<expressions::ExpressionBase> updateExpr,
        std::unique_ptr<StatementBase> statement);

   private:
    IterationStatementType _type;
    std::unique_ptr<StatementBase> _statement;
    std::unique_ptr<ExpressionStatement> _initExpr;
    std::unique_ptr<ExpressionStatement> _condExpr;
    std::unique_ptr<expressions::ExpressionBase> _updateExpr;
};

}  // namespace thogcc::ast::statements
