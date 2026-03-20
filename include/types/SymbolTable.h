#pragma once

#include <cassert>
#include <format>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "types/Scope.h"

namespace thogcc::types {

class SymbolTable {
   public:
    SymbolTable() {
        pushScope();  // global scope
    };

    void pushScope() {
        _scopes.emplace_back();
    };
    void popScope() {
        assert(!_scopes.empty());
        _scopes.pop_back();
    };

    void addToScope(std::string identifier, OrdSymbol symbol) {
        _scopes.back().ordinarySymbols.insert({std::move(identifier), std::move(symbol)});
    }

    void addToParentScope(std::string identifier, OrdSymbol symbol) {
        _scopes.at(_scopes.size() - 2)
            .ordinarySymbols.insert({std::move(identifier), std::move(symbol)});
    }

    const OrdSymbol& getOrd(const std::string& identifier) const {
        for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
            auto a = it->ordinarySymbols.find(identifier);
            if (a != it->ordinarySymbols.end()) {
                return a->second;
            }
        }
        throw std::runtime_error(std::format("Symbol {} doesn't exist.", identifier));
    }

    const Scope& getBack() const {
        return _scopes.back();
    }

   private:
    std::vector<Scope> _scopes;
};

}  // namespace thogcc::types
