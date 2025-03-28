#include "node.h"

#include <algorithm>
#include <cassert>
#include <glm/ext/matrix_transform.hpp>
#include <stdexcept>
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

const Matrix4& Node::GetReverseTransform() const {
    return reverse_transform_;
}

Matrix4 Node::GetGlobalTransform() const {
    Matrix4 result(1.0);
    const Node* temp = this;
    while (temp->parent_ != nullptr) {
        result = temp->GetTransform() * result;
        temp = temp->GetParent();
    }
    return result;
}

Matrix4 Node::GetGlobalReverseTransform() const {
    Matrix4 result(1.0);
    const Node* temp = this;
    while (temp->parent_ != nullptr) {
        result = temp->GetReverseTransform() * result;
        temp = temp->GetParent();
    }
    return result;
}

void Node::SetPosition(Vector3 new_pos) {
    transform_ = glm::translate<Real>(transform_, new_pos);
    reverse_transform_ = glm::translate<Real>(Matrix4(1.0), -new_pos) * reverse_transform_;
}

Vector3 Node::GetPosition() const {
    return GetGlobalTransform()[3];
}

void Node::SetRotationX(Real angle) {
    transform_ = glm::rotate<Real>(transform_, glm::radians(angle), Vector3(1.0, 0.0, 0.0));
    reverse_transform_ =
        glm::rotate<Real>(Matrix4(1.0), glm::radians(-angle), Vector3(1.0, 0.0, 0.0)) *
        reverse_transform_;
}

void Node::SetRotationY(Real angle) {
    transform_ = glm::rotate<Real>(transform_, glm::radians(angle), Vector3(0.0, 1.0, 0.0));
    reverse_transform_ =
        glm::rotate<Real>(Matrix4(1.0), glm::radians(-angle), Vector3(0.0, 1.0, 0.0)) *
        reverse_transform_;
}

void Node::SetRotationZ(Real angle) {
    transform_ = glm::rotate<Real>(transform_, glm::radians(angle), Vector3(0.0, 0.0, 1.0));
    reverse_transform_ =
        glm::rotate<Real>(Matrix4(1.0), glm::radians(-angle), Vector3(0.0, 0.0, 1.0)) *
        reverse_transform_;
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
