#include "node.h"

#include <algorithm>
#include <cassert>

namespace engine::details {

Node::Node(const Node& other)
    : data_{other.data_}, parent_{other.parent_}, position_{other.position_} {
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
    std::swap(position_, other.position_);
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

void Node::SetParent(Node& node) noexcept {
    assert(!node.HasParent(*this) && "Node: Tried to link parent to its child");
    Unlink();
    parent_ = &node;
    node.children_.emplace_back(this);
}

void Node::AddChild(Node& node) noexcept {
    node.SetParent(*this);
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

void Node::SetPosition(Vector3 new_pos) {
    position_ = new_pos;
}

Vector3 Node::GetPosition() const {
    return position_;
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
