#pragma once
#include "camera.h"
#include "geometry.h"
#include "object3d.h"

#include <glm/vec3.hpp>

#include <memory>
#include <variant>
#include <vector>

namespace engine {
using EmptyNode = std::monostate;
}  // namespace engine

namespace engine::details {

class Node {
    using NodeDataType = std::variant<EmptyNode, Camera, Object3D>;

public:
    using IndexType = size_t;

    Node() = default;

    template <typename T>
        requires(!std::is_same_v<std::decay_t<T>, Node>)
    Node(T&& data) : data_{data} {
    }

    // Copies Node with its subtree
    Node(const Node& other);
    Node& operator=(const Node& other);
    // Other Nodes' pointers break if Node is moved
    Node(Node&& other) = delete;
    Node& operator=(Node&& other) = delete;
    // Swaps Nodes' subtrees
    void Swap(Node& other);

    Node* GetParent();
    const Node* GetParent() const;

    IndexType GetChildCount() const;
    Node* GetChild(IndexType id);
    const Node* GetChild(IndexType id) const;

    void SetParent(Node& node) noexcept;
    void AddChild(Node& node) noexcept;
    void Unlink() noexcept;

    void SetPosition(Vector3 new_pos);
    Vector3 GetPosition() const;

    template <typename T>
    friend bool Is(const Node& node) {
        return (std::get_if<T>(&node.data_) != nullptr);
    }

    template <typename T>
    friend const T* As(const Node& node) {
        return &std::get<T>(node.data_);
    }

    template <typename T>
    friend T* As(Node& node) {
        return &std::get<T>(node.data_);
    }

private:
    // Invariants
    bool HasParent(const Node& other_node) const;

    NodeDataType data_ = EmptyNode{};
    Node* parent_ = nullptr;
    std::vector<std::unique_ptr<Node>> children_ = {};

    // TODO: implement Transforms logic
    Vector3 position_ = Node::kDefaultPosition;

    static constexpr Vector3 kDefaultPosition = {0, 0, 0};
};

}  // namespace engine::details