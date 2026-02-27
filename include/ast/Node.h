#pragma once

#include <concepts>
#include <memory>
#include <vector>

namespace thogcc::ast {

class Node {
   public:
    virtual ~Node();
};

template <std::derived_from<Node> T>
class NodeList : public T {
   public:
    NodeList(std::unique_ptr<T> node);
    void pushBack(T node);

   private:
    std::vector<std::unique_ptr<T>> _nodes;
};

}  // namespace thogcc::ast
