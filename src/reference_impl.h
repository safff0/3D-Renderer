#pragma once
#include "alias.h"
#include "node.h"

namespace engine {

namespace details {

template <typename T>
concept NoQualifiers = (std::is_same_v<T, std::decay_t<T>>);

struct ConstValueTag {};
struct NonConstValueTag {};

template <typename T>
concept ConstTag = (std::is_same_v<T, ConstValueTag> || std::is_same_v<T, NonConstValueTag>);

template <ConstTag IsConst>
struct NodeTypes;

template <>
struct NodeTypes<ConstValueTag> {
    using NodeType = const Node;
};

template <>
struct NodeTypes<NonConstValueTag> {
    using NodeType = Node;
};

template <ConstTag IsConst>
class NodeController : private NodeTypes<IsConst> {
    template <ConstTag OtherConst>
    friend class NodeController;

public:
    using typename NodeTypes<IsConst>::NodeType;

    NodeController() = default;

    explicit NodeController(NodeType* node) : node_{node} {
        assert(node != nullptr && "Reference: Tried to refer to nullptr");
    }

protected:
    NodeType* node_;
};

template <NoQualifiers DataT, ConstTag IsConst>
class ReferenceImpl;

template <ConstTag IsConst>
class ReferenceImpl<EmptyNode, IsConst> : protected NodeController<IsConst> {
    template <NoQualifiers OtherDataT, ConstTag OtherConst>
    friend class ReferenceImpl;

    template <NoQualifiers T1, ConstTag C1, NoQualifiers T2, ConstTag C2>
    friend bool operator==(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2);

    template <NoQualifiers T1, ConstTag C1, NoQualifiers T2, ConstTag C2>
    friend bool operator!=(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2);

    using Base = NodeController<IsConst>;
    using Base::node_;
    using IndexType = Node::IndexType;

public:
    using typename Base::NodeType;

    ReferenceImpl() = default;

    explicit ReferenceImpl(NodeType* node) : Base(node) {
    }

    template <typename T, ConstTag OtherConst>
        requires(std::is_same_v<IsConst, OtherConst> || std::is_same_v<IsConst, ConstValueTag>)
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
        requires(std::is_same_v<IsConst, NonConstValueTag>)
    void SetParent(ReferenceImpl<T, NonConstValueTag> ref) {
        node_->SetParent(*ref.node_);
    }

    template <typename T>
        requires(std::is_same_v<IsConst, NonConstValueTag>)
    void AddChild(ReferenceImpl<T, NonConstValueTag> ref) {
        node_->AddChild(*ref.node_);
    }

    template <typename T>
        requires(std::is_same_v<IsConst, NonConstValueTag>)
    ReferenceImpl<T, IsConst> NewChild(T&& data, Vector3 position = Node::kDefaultPosition) {
        return ReferenceImpl<T, IsConst>{
            node_->template NewChild<T>(std::forward<T>(data), position)};
    }

    template <typename = std::enable_if<std::is_same_v<IsConst, NonConstValueTag>>>
    void DeleteSubtree() noexcept {
        node_->Unlink();
        delete node_;
    }

    template <typename = std::enable_if<std::is_same_v<IsConst, NonConstValueTag>>>
    void SetPosition(Vector3 new_position) {
        node_->SetPosition(new_position);
    }

    Vector3 GetPosition() {
        return node_->GetPosition();
    }

    const Matrix4& GetLocalTransform() {
        return node_->GetLocalTransform();
    }

    const Matrix4& GetLocalReverseTransform() {
        return node_->GetLocalReverseTransform();
    }

    Matrix4 GetGlobalTransform() {
        return node_->GetGlobalTransform();
    }

    Matrix4 GetGlobalReverseTransform() {
        return node_->GetGlobalReverseTransform();
    }

    template <typename = std::enable_if<std::is_same_v<IsConst, NonConstValueTag>>>
    void SetRotationOnAxis(Real angle, Vector3 axis) {
        node_->SetRotationOnAxis(angle, axis);
    }

    template <typename = std::enable_if<std::is_same_v<IsConst, NonConstValueTag>>>
    void SetRotationX(Real angle) {
        node_->SetRotationX(angle);
    }

    template <typename = std::enable_if<std::is_same_v<IsConst, NonConstValueTag>>>
    void SetRotationY(Real angle) {
        node_->SetRotationY(angle);
    }

    template <typename = std::enable_if<std::is_same_v<IsConst, NonConstValueTag>>>
    void SetRotationZ(Real angle) {
        node_->SetRotationZ(angle);
    }

    template <typename T>
    friend bool Is(ReferenceImpl<EmptyNode, IsConst> ref) {
        return Is<T>(*ref.node_);
    }

    template <typename T>
    friend ReferenceImpl<T, IsConst> As(ReferenceImpl<EmptyNode, IsConst> ref) {
        return static_cast<ReferenceImpl<T, IsConst>>(ref);
    }
};

template <NoQualifiers T, ConstTag IsConst>
struct ReferenceTypes;

template <NoQualifiers T>
struct ReferenceTypes<T, ConstValueTag> {
    using DataType = const T;
};

template <NoQualifiers T>
struct ReferenceTypes<T, NonConstValueTag> {
    using DataType = T;
};

template <NoQualifiers DataT, ConstTag IsConst>
class ReferenceImpl : public ReferenceImpl<EmptyNode, IsConst>,
                      private ReferenceTypes<DataT, IsConst> {
    template <NoQualifiers OtherDataT, ConstTag OtherConst>
    friend class ReferenceImpl;

    template <NoQualifiers T1, ConstTag C1, NoQualifiers T2, ConstTag C2>
    friend bool operator==(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2);

    template <NoQualifiers T1, ConstTag C1, NoQualifiers T2, ConstTag C2>
    friend bool operator!=(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2);

    using Base = NodeController<IsConst>;
    using Base::node_;

public:
    using typename ReferenceTypes<DataT, IsConst>::DataType;
    using typename Base::NodeType;

    ReferenceImpl() = default;

    ReferenceImpl(NodeType* node) : ReferenceImpl<EmptyNode, IsConst>(node) {
        assert(Is<DataT>(*node_) && "Reference: Data and Reference types do not match");
    }

    template <ConstTag OtherConst>
        requires(std::is_same_v<IsConst, OtherConst> || std::is_same_v<IsConst, ConstValueTag>)
    ReferenceImpl(ReferenceImpl<DataT, OtherConst> ref) : ReferenceImpl(ref.node_) {
    }

    template <ConstTag OtherConst>
        requires(std::is_same_v<IsConst, OtherConst> || std::is_same_v<IsConst, ConstValueTag>)
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

template <NoQualifiers T1, ConstTag C1, NoQualifiers T2, ConstTag C2>
bool operator==(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2) {
    return ref1.node_ == ref2.node_;
}

template <NoQualifiers T1, ConstTag C1, NoQualifiers T2, ConstTag C2>
bool operator!=(ReferenceImpl<T1, C1> ref1, ReferenceImpl<T2, C2> ref2) {
    return !(ref1 == ref2);
}

}  // namespace details

}  // namespace engine
