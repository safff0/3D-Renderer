#include "node.h"

#include <algorithm>
#include <cassert>
#include "alias.h"

namespace engine::details {

Node::Node(const Node& other)
    : data_{other.data_}, parent_{other.parent_}, transform_{other.transform_} {
    // Copy subtree
    for (const std::unique_ptr<Node>& subtree : other.children_) {
        children_.push_back(std::make_unique<Node>(*subtree));
    }
}

Node& Node::operator=(const Node& other) {
    Node tmp(other);
    Swap(tmp);
    return *this;
}

void Node::Swap(Node& other) {
    children_.swap(other.children_);
    std::swap(parent_, other.parent_);
    std::swap(data_, other.data_);
    std::swap(transform_, other.transform_);
}

Node* Node::GetParent() {
    assert(parent_ != nullptr && "Node: Tried to get nonexistent parent");
    return parent_;
}

const Node* Node::GetParent() const {
    assert(parent_ != nullptr && "Node: Tried to get nonexistent parent");
    return parent_;
}

Node::IndexType Node::GetChildCount() const {
    return children_.size();
}

Node* Node::GetChild(IndexType id) {
    assert(id < children_.size() && "Node: Child index out of range");
    return children_[id].get();
}

const Node* Node::GetChild(IndexType id) const {
    assert(id < children_.size() && "Node: Child index out of range");
    return children_[id].get();
}

void Node::SetParent(Node& node) noexcept {
    assert(!node.HasParent(*this) && "Node: Tried to link parent to its child");
    Unlink();
    parent_ = &node;
    node.children_.emplace_back(this);
}

void Node::AddChild(Node& node) noexcept {
    node.SetParent(*this);
}

void Node::Unlink() noexcept {
    if (parent_ != nullptr) {
        Node* parent = GetParent();
        auto it = std::find_if(parent->children_.begin(), parent->children_.end(),
                               [this](std::unique_ptr<Node>& ptr) { return ptr.get() == this; });
        assert(it != parent->children_.end() && "Node: Parent does not have info about child");
        it->release();
        parent->children_.erase(it);
    }
}

const Matrix4& Node::GetTransform() const {
    return transform_;
}

void Node::SetPosition(Vector3 new_pos) {
    transform_ = glm::translate<Real>(transform_, new_pos);
}

void Node::SetRotationX(Real angle) {
    transform_ = glm::rotate<Real>(transform_, glm::radians(angle), Vector3(1.0f, 0.0f, 0.0f));
}

void Node::SetRotationY(Real angle) {
    transform_ = glm::rotate<Real>(transform_, glm::radians(angle), Vector3(0.0f, 1.0f, 0.0f));
}

void Node::SetRotationZ(Real angle) {
    transform_ = glm::rotate<Real>(transform_, glm::radians(angle), Vector3(0.0f, 0.0f, 1.0f));
}

Vector3 Node::GetPosition() const {
    const Node* root = this;
    std::vector<const Node*> path;
    while (root) {
        path.push_back(root);
        root = root->parent_;
    }
    std::reverse(path.begin(), path.end());
    Matrix4 global_transform = kDefaultTransform;
    for (auto& node : path) {
        global_transform *= node->transform_;
    }
    return global_transform[3];
}

bool Node::HasParent(const Node& other_node) const {
    const Node* parent = this;
    while (parent != nullptr) {
        if (parent == &other_node) {
            return true;
        }
        parent = parent->parent_;
    }
    return false;
}

}  // namespace engine::details
