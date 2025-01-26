#pragma once
#include "node.h"

namespace engine {

namespace details {

template <typename T>
concept NoQualifiers = (std::is_same_v<T, std::decay_t<T>>);

template <bool IsConst>
struct NodeTypes;

template <>
struct NodeTypes<true> {
    using NodeType = const Node;
};

template <>
struct NodeTypes<false> {
    using NodeType = Node;
};

template <bool IsConst>
class NodeController : private NodeTypes<IsConst> {
    template <bool OtherConst>
    friend class NodeController;

public:
    using typename NodeTypes<IsConst>::NodeType;

    NodeController(NodeType* node) : node_{node} {
        assert(node != nullptr && "Reference: Tried to refer to nullptr");
    }

protected:
    NodeType* node_;
};

template <NoQualifiers DataT, bool IsConst>
class ReferenceImpl;

template <bool IsConst>
class ReferenceImpl<EmptyNode, IsConst> : protected NodeController<IsConst> {
    template <NoQualifiers OtherDataT, bool OtherConst>
    friend class ReferenceImpl;

    using Base = NodeController<IsConst>;
    using Base::node_;
    using IndexType = Node::IndexType;

public:
    using typename Base::NodeType;

    ReferenceImpl() = delete;

    ReferenceImpl(NodeType* node) : Base(node) {
    }

    template <typename T, bool OtherConst>
        requires(IsConst == OtherConst || IsConst == true)
    ReferenceImpl(ReferenceImpl<T, OtherConst> ref) : ReferenceImpl(ref.node_) {
    }

    template <typename T = EmptyNode>
    ReferenceImpl<T, IsConst> GetParent() {
        return ReferenceImpl<T, IsConst>{node_->GetParent()};
    }

    template <typename T = EmptyNode>
    ReferenceImpl<T, IsConst> GetChild(IndexType index) {
        return ReferenceImpl<T, IsConst>{node_->GetChild(index)};
    }

    IndexType GetChildCount() {
        return node_->GetChildCount();
    }

    template <typename T>
        requires(!IsConst)
    void SetParent(ReferenceImpl<T, false> ref) {
        node_->SetParent(*ref.node_);
    }

    template <typename T>
        requires(!IsConst)
    void AddChild(ReferenceImpl<T, false> ref) {
        node_->AddChild(*ref.node_);
    }

    template <typename = std::enable_if<!IsConst>>
    void DeleteSubtree() noexcept {
        node_->Unlink();
        delete node_;
    }

    template <typename = std::enable_if<!IsConst>>
    void SetPosition(Vector3 new_position) {
        node_->SetPosition(new_position);
    }

    Vector3 GetPosition() {
        return node_->GetPosition();
    }

    template <typename T>
    friend bool Is(ReferenceImpl<EmptyNode, IsConst> ref) {
        return Is<T>(*ref.node_);
    }
};

template <NoQualifiers T, bool IsConst>
struct ReferenceTypes;

template <NoQualifiers T>
struct ReferenceTypes<T, true> {
    using DataType = const T;
};

template <NoQualifiers T>
struct ReferenceTypes<T, false> {
    using DataType = T;
};

template <NoQualifiers DataT, bool IsConst>
class ReferenceImpl : public ReferenceImpl<EmptyNode, IsConst>,
                      private ReferenceTypes<DataT, IsConst> {
    template <NoQualifiers OtherDataT, bool OtherConst>
    friend class ReferenceImpl;

    template <NoQualifiers T1, bool C1, NoQualifiers T2, bool C2>
    friend bool operator==(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2);

    template <NoQualifiers T1, bool C1, NoQualifiers T2, bool C2>
    friend bool operator!=(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2);

    using Base = NodeController<IsConst>;
    using Base::node_;

public:
    using typename ReferenceTypes<DataT, IsConst>::DataType;
    using typename Base::NodeType;

    ReferenceImpl() = delete;

    ReferenceImpl(NodeType* node) : ReferenceImpl<EmptyNode, IsConst>(node) {
        assert(Is<DataT>(*node_) && "Reference: Data and Reference types do not match");
    }

    template <bool OtherConst>
        requires(IsConst == OtherConst || IsConst == true)
    ReferenceImpl(ReferenceImpl<DataT, OtherConst> ref) : ReferenceImpl(ref.node_) {
    }

    template <bool OtherConst>
        requires(IsConst == OtherConst || IsConst == true)
    ReferenceImpl(ReferenceImpl<EmptyNode, OtherConst> ref) : ReferenceImpl(ref.node_) {
        assert(Is<DataT>(*ref.node_) && "Reference: Invalid type promotion");
    }

    DataType* operator->() const {
        return As<DataT>(*node_);
    }

    DataType& operator*() const {
        return *As<DataT>(*node_);
    }
};

template <NoQualifiers T1, bool C1, NoQualifiers T2, bool C2>
bool operator==(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2) {
    return ref1.node_ == ref2.node_;
}

template <NoQualifiers T1, bool C1, NoQualifiers T2, bool C2>
bool operator!=(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2) {
    return !(ref1 == ref2);
}

}  // namespace details

}  // namespace engine