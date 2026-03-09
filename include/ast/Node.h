#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/StorageClassSpecifier.h"
#include "ast/TypeSpecifier.h"
#include "ast/utils.h"
#include "visitors/Visitor.h"

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

class NodeListBase : public Node {
   public:
    using BaseType = Node;
    virtual size_t size() const = 0;
    virtual Node& getRawNode(size_t index) const = 0;
};

template <typename T>
class NodeList : public NodeListBase {  // Yes, this is correct. We don't use VisitableNode, because
                                        // we manually override getKind and accept
   public:
    NodeList() = default;
    NodeList(std::unique_ptr<T> node) {
        static_assert(std::derived_from<T, Node>, "T must derive from Node");
        _nodes.push_back(std::move(node));
    };

    void pushBack(std::unique_ptr<T> node) {
        if (node) _nodes.push_back(std::move(node));
    };
    auto& getNodes() const {
        return _nodes;
    };

    size_t size() const override {
        return _nodes.size();
    };
    Node& getRawNode(size_t index) const override {
        return *_nodes[index];
    };

    using BaseType = NodeListBase;
    NodeKind getKind() const override {
        // Every NodeList<T> is a NodeListBase
        return NodeKind::NodeListBase;
    }
    void accept(visitors::Visitor& v) override {
        // NodeList<T>::accept() calls visit(NodeListBase& node) for all T
        v.visit(static_cast<NodeListBase&>(*this));
    }

   private:
    std::vector<std::unique_ptr<Node>> _nodes;
};

class ValueNodeBase : public Node {
   public:
    using BaseType = Node;
    virtual std::string getLabel() = 0;
};

template <typename E>
class ValueNode
    : public ValueNodeBase {  // Same as NodeList, we will override getKind, accept ourselves
   public:
    ValueNode(E value) : _value(value){};
    std::string getLabel() override {
        if constexpr (std::is_same_v<E, TypeSpecifier>) {
            switch (_value) {
#define X(VAL)   \
    case E::VAL: \
        return #VAL;
                TYPE_SPECIFIER
#undef X
            }
        } else if constexpr (std::is_same_v<E, StorageClassSpecifier>) {
            switch (_value) {
#define X(VAL)   \
    case E::VAL: \
        return #VAL;
                STORAGE_CLASS_SPECIFIER
#undef X
            }
        } else if constexpr (std::is_convertible_v<E, std::string>) {
            return _value;
        } else {
            static_assert(always_false<E>, "fuck");
        }
    };

    using BaseType = ValueNodeBase;
    NodeKind getKind() const override {
        return NodeKind::ValueNodeBase;
    }
    void accept(visitors::Visitor& v) override {
        v.visit(static_cast<ValueNodeBase&>(*this));
    }

   private:
    E _value;
};

}  // namespace thogcc::ast
