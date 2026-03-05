#include "ast/declarators/PointerDeclarator.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include "ast/declarators/DeclaratorBase.h"

namespace thogcc::ast::declarators {

void PointerDeclarator::attach(std::unique_ptr<DeclaratorBase> node) {
    PointerDeclarator* current = this;

    while (current->_ptr) {
        if (auto* child = dynamic_cast<PointerDeclarator*>(current->_ptr.get())) {
            current = child;
        } else {
            throw std::runtime_error("attach() called on PointerDeclarator with existing child");
        }
    }

    current->_ptr = std::move(node);
};

}  // namespace thogcc::ast::declarators
