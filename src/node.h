#pragma once
#include "alias.h"
#include "camera.h"
#include "geometry.h"
#include "light.h"
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
    using NodeDataType = std::variant<EmptyNode, Camera, Object3D, LightSource>;

public:
    using IndexType = Index;

    Node() = default;

    template <typename T>
        requires(!std::is_same_v<std::decay_t<T>, Node>)
    Node(T&& data) : data_{std::forward<T>(data)} {
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

    template <typename T>
    Node* NewChild(T&& data) noexcept {
        children_.push_back(std::make_unique<Node>(std::forward<T>(data)));
        children_.back()->parent_ = this;
        return children_.back().get();
    }

    const Matrix4& GetTransform() const;
    const Matrix4& GetReverseTransform() const;
    const Matrix4& GetGlobalTransform() const;
    const Matrix4& GetGlobalReverseTransform() const;

    void SetPosition(Vector3 new_pos);
    Vector3 GetPosition() const;

    void SetRotationOnAxis(Real angle, Vector3 axis);
    void SetRotationX(Real angle);
    void SetRotationY(Real angle);
    void SetRotationZ(Real angle);

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
    static constexpr Matrix4 kDefaultTransform = Matrix4(1.0f);

    void UpdateSubtreeTransform();
    // Invariants
    bool HasParent(const Node& other_node) const;

    NodeDataType data_ = EmptyNode{};
    Node* parent_ = nullptr;
    std::vector<std::unique_ptr<Node>> children_ = {};

    Matrix4 transform_ = kDefaultTransform;
    Matrix4 reverse_transform_ = kDefaultTransform;
    Matrix4 g_transform_ = kDefaultTransform;
    Matrix4 g_reverse_transform_ = kDefaultTransform;
};

}  // namespace engine::details
