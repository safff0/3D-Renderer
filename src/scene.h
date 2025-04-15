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
    Reference<EmptyNode> GetRoot();

private:
    std::unique_ptr<Node> root_ = std::make_unique<Node>();
};

}  // namespace engine
