#include "scene.h"
#include "node.h"
#include "reference.h"

namespace engine {

Scene::Scene(const Scene& other) : root_{other.root_->CopySubtree()} {
}

Scene& Scene::operator=(const Scene& other) {
    Scene tmp(other);
    Swap(tmp);
    return *this;
}

void Scene::Swap(Scene& other) {
    root_.swap(other.root_);
}

ConstReference<EmptyNode> Scene::GetRoot() const {
    return ConstReference<EmptyNode>{root_.get()};
}

Reference<EmptyNode> Scene::GetRoot() {
    return Reference<EmptyNode>{root_.get()};
}

}  // namespace engine
