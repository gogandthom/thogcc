#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace thogcc::ast {

class Node {
   public:
    Node() = default;
    virtual ~Node() = default;
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    virtual void accept(visitors::Visitor& v) = 0;
    virtual NodeKind getKind() const = 0;
};

template <typename Derived, typename Base = Node>
class VisitableNode : public Base {
   public:
    using Base::Base;  // Keep Base constructor
    using BaseType = Base;

    static constexpr NodeKind kind = NodeKindTrait<Derived>::kind;

    NodeKind getKind() const override {
        return kind;
    };
    void accept(visitors::Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    }
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
