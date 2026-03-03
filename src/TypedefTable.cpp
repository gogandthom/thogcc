#include "TypedefTable.h"

#include <string>

namespace thogcc {

void TypedefTable::pushScope() {
    _scopes.push_back({});
}

void TypedefTable::popScope() {
    _scopes.pop_back();
}

void TypedefTable::addType(const std::string& identifier) {
    if (_scopes.empty()) pushScope();
    _scopes.back().insert(identifier);
}

bool TypedefTable::isType(const std::string& identifier) {
    for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
        if (it->contains(identifier)) return true;
    }
    return false;
}

}  // namespace thogcc
