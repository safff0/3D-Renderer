#pragma once
#include "reference_impl.h"

/*
    Ref<EmptyNode> behaves like a Node reference (can be used to change parent/position etc.)

    Ref<EngineType> inherits Ref<EmptyNode> interface but also has ->, * operators to change
    EngineType stored in it directly. It can be cast to Ref<EmptyNode>

    All Ref's have const variations (i.e. ConstReference<T>)
*/
namespace engine {

template <details::NoQualifiers T>
using Reference = details::ReferenceImpl<T, false>;

template <details::NoQualifiers T>
using ConstReference = details::ReferenceImpl<T, true>;

}  // namespace engine
