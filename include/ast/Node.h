#pragma once

#include <concepts>
#include <memory>
#include <vector>

namespace thogcc::ast {

class Node {
   public:
    virtual ~Node() = default;
};

template <typename T>
class NodeList : public Node {
   public:
    NodeList(std::unique_ptr<T> node);
    void pushBack(std::unique_ptr<T> node);

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

template <typename T>
NodeList<T>::NodeList(std::unique_ptr<T> node) {
    static_assert(std::derived_from<T, Node>, "T must derive from Node");
    _nodes.push_back(std::move(node));
};

template <typename T>
void NodeList<T>::pushBack(std::unique_ptr<T> node) {
    _nodes.push_back(std::move(node));
};

}  // namespace thogcc::ast
