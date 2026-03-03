#include "ast/declarators/PointerDeclarator.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

void PointerDeclarator::attach(std::unique_ptr<DeclaratorBase> node) {
    if (!_ptr) {
        _ptr = std::move(node);
    } else {
        if (auto child = dynamic_cast<PointerDeclarator*>(_ptr.get())) {
            child->attach(std::move(node));
        } else {
            throw std::runtime_error("attach() called on PointerDeclarator with existing child");
        }
    }
};

}  // namespace thogcc::ast::declarators
