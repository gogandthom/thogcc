#pragma once

#include <concepts>
#include <memory>
#include <vector>

namespace thogcc::ast {

class Node {
   public:
    virtual ~Node() = default;
};

template <std::derived_from<Node> T>
class NodeList : public Node {
   public:
    NodeList(std::unique_ptr<T> node);
    void pushBack(T node);

   private:
    std::vector<std::unique_ptr<T>> _nodes;
};

template <typename E>
class ValueNode : public Node {
   public:
    ValueNode(E value) : _value(value){};

   protected:
    E _value;
};

}  // namespace thogcc::ast
