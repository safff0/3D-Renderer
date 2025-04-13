#include "node.h"
#include "alias.h"
#include "geometry.h"

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace engine::details {

Node::Node(const Node& other)
    : data_{other.data_},
      parent_{other.parent_},
      transform_{other.transform_},
      reverse_transform_{other.reverse_transform_},
      g_transform_{other.g_transform_},
      g_reverse_transform_{other.g_reverse_transform_} {
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
    assert(!other.HasParent(*this) && !HasParent(other) &&
           "Node: Tried to link parent to its child");
    children_.swap(other.children_);
    std::swap(parent_, other.parent_);
    std::swap(data_, other.data_);
    std::swap(transform_, other.transform_);
    UpdateSubtreeTransform();
    other.UpdateSubtreeTransform();
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
    UpdateSubtreeTransform();
}

void Node::AddChild(Node& node) noexcept {
    node.SetParent(*this);
    node.UpdateSubtreeTransform();
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

const Matrix4& Node::GetReverseTransform() const {
    return reverse_transform_;
}

const Matrix4& Node::GetGlobalTransform() const {
    return g_transform_;
}

const Matrix4& Node::GetGlobalReverseTransform() const {
    return g_reverse_transform_;
}

void Node::SetPosition(Vector3 new_pos) {
    new_pos = PointApplyTransform(new_pos, transform_);
    new_pos -= transform_ * Vector4(0, 0, 0, 1);
    transform_ = glm::translate<Real>(Matrix4(1.0), new_pos) * transform_;
    reverse_transform_ = reverse_transform_ * glm::translate<Real>(Matrix4(1.0), -new_pos);
    UpdateSubtreeTransform();
}

Vector3 Node::GetPosition() const {
    return GetGlobalTransform() * Vector4(0, 0, 0, 1);
}

void Node::SetRotationOnAxis(Real angle, Vector3 axis) {
    axis = PointApplyTransform(axis, transform_);
    axis -= transform_ * Vector4(0, 0, 0, 1);
    transform_ = glm::rotate<Real>(Matrix4(1), glm::radians(angle), axis) * transform_;
    reverse_transform_ =
        reverse_transform_ * glm::rotate<Real>(Matrix4(1.0), -glm::radians(angle), axis);
    UpdateSubtreeTransform();
}

void Node::SetRotationX(Real angle) {
    SetRotationOnAxis(angle, {1, 0, 0});
}

void Node::SetRotationY(Real angle) {
    SetRotationOnAxis(angle, {0, 1, 0});
}

void Node::SetRotationZ(Real angle) {
    SetRotationOnAxis(angle, {0, 0, 1});
}

void Node::UpdateSubtreeTransform() {
    if (parent_ == nullptr) {
        g_transform_ = g_reverse_transform_ = kDefaultTransform;
    } else {
        g_transform_ = transform_ * GetParent()->GetGlobalTransform();
        g_reverse_transform_ = reverse_transform_ * GetParent()->GetGlobalReverseTransform();
    }
    for (auto& child : children_) {
        child->UpdateSubtreeTransform();
    }
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
