#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "ast/declarations/ParameterDeclaration.h"
#include "ast/declarators/DeclaratorBase.h"
#include "ast/declarators/IdentifierDeclarator.h"

namespace thogcc::ast::declarators {

enum class FunctionDeclaratorForm : std::uint8_t { Prototype, KAndR };

class FunctionDeclarator : public DeclaratorBase {
   public:
    FunctionDeclarator(
        std::unique_ptr<DeclaratorBase> base = nullptr,
        std::unique_ptr<NodeList<declarations::ParameterDeclaration>> params = nullptr)
        : _form(FunctionDeclaratorForm::Prototype),
          _base(std::move(base)),
          _params(std::move(params)){};
    FunctionDeclarator(std::unique_ptr<DeclaratorBase> base,
                       std::unique_ptr<NodeList<IdentifierDeclarator>> identifiers)
        : _form(FunctionDeclaratorForm::KAndR),
          _base(std::move(base)),
          _identifiers(std::move(identifiers)){};

   private:
    FunctionDeclaratorForm _form;
    std::unique_ptr<DeclaratorBase> _base;
    std::unique_ptr<NodeList<declarations::ParameterDeclaration>> _params;
    std::unique_ptr<NodeList<IdentifierDeclarator>> _identifiers;
};

}  // namespace thogcc::ast::declarators
