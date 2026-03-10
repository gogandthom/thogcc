#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace thogcc {

class TypedefTable {
   public:
    void pushScope() {
        _scopes.push_back({});
    }
    void popScope() {
        _scopes.pop_back();
    }
    void addType(std::string identifier) {
        if (_scopes.empty()) pushScope();
        _scopes.back().insert(std::move(identifier));
    }
    bool isType(const std::string& identifier) {
        for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
            if (it->contains(identifier)) return true;
        }
        return false;
    }

   private:
    std::vector<std::unordered_set<std::string>> _scopes;
};

}  // namespace thogcc
