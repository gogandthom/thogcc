#pragma once

#include <cassert>
#include <string>
#include <unordered_set>
#include <vector>

namespace thogcc {

class TypedefTable {
   public:
    TypedefTable() {
        pushScope();  // Global scope
    }

    void pushScope() {
        _scopes.emplace_back();
    }
    void popScope() {
        assert(!_scopes.empty());
        _scopes.pop_back();
    }
    void addType(std::string identifier) {
        if (_scopes.empty()) pushScope();
        _scopes.back().emplace(identifier);
    }
    bool isType(const std::string& identifier) const {
        for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
            if (it->contains(identifier)) return true;
        }
        return false;
    }

   private:
    std::vector<std::unordered_set<std::string>> _scopes;
};

}  // namespace thogcc
