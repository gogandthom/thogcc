#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace thogcc {

class TypedefTable {
   public:
    void pushScope();
    void popScope();
    void addType(const std::string& identifier);
    bool isType(const std::string& identifier);

   private:
    std::vector<std::unordered_set<std::string>> _scopes;
};

}  // namespace thogcc
