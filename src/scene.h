#pragma once
#include "node.h"
#include "reference.h"

namespace engine {

class Scene {
public:
    using Node = details::Node;
    using IndexType = details::Node::IndexType;

    Scene() = default;
    Scene(const Scene& other);
    Scene& operator=(const Scene& other);
    Scene(Scene&& other) noexcept = default;
    Scene& operator=(Scene&& other) noexcept = default;

    void Swap(Scene& other);

    ConstReference<EmptyNode> GetRoot() const;

    template <typename T, typename... Args>
    Reference<T> NewNode(Args&&... args) noexcept {
        Node* new_node = new Node{T{std::forward<Args>(args)...}};
        new_node->SetParent(*root_);
        return Reference<T>{new_node};
    }

private:
    std::unique_ptr<Node> root_ = std::make_unique<Node>();
};

}  // namespace engine
