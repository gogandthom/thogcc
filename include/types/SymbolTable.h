#pragma once

#include <cassert>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
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

    void addToScope(std::string identifier, std::shared_ptr<OrdSymbol> symbol) {
        _scopes.back().ordinarySymbols.insert({std::move(identifier), std::move(symbol)});
    }

    void addToParentScope(std::string identifier, std::shared_ptr<OrdSymbol> symbol) {
        _scopes.at(_scopes.size() - 2)
            .ordinarySymbols.insert({std::move(identifier), std::move(symbol)});
    }

    std::shared_ptr<OrdSymbol> getOrd(const std::string& identifier) {
        for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
            auto a = it->ordinarySymbols.find(identifier);
            if (a != it->ordinarySymbols.end()) {
                return a->second;
            }
        }
        throw std::runtime_error(std::format("Symbol {} doesn't exist.", identifier));
    }

   private:
    std::vector<Scope> _scopes;
};

}  // namespace thogcc::types
