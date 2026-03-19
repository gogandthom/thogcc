#pragma once

#include <memory>
#include <utility>

#include "ast/Node.h"
#include "types/Type.h"

namespace thogcc::ast::expressions {

class ExpressionBase : public VisitableNode<ExpressionBase> {
   public:
    void setEvaluatedType(std::shared_ptr<types::Type> symb) {
        _evaluatedType = std::move(symb);
    }
    auto getEvaluatedType() const {
        return _evaluatedType;
    }
    void setIsLvalue(bool isLvalue) {
        _isLvalue = isLvalue;
    }
    bool isLvalue() const {
        return _isLvalue;
    }

   private:
    std::shared_ptr<types::Type> _evaluatedType;
    bool _isLvalue;
};

}  // namespace thogcc::ast::expressions
