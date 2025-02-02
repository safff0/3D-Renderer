#include "scene.h"
#include "node.h"
#include "reference.h"

namespace engine {

ConstReference<EmptyNode> Scene::GetRoot() const {
    return ConstReference<EmptyNode>{root_.get()};
}

Scene::Scene(const Scene& other) : root_{std::make_unique<details::Node>(*other.root_)} {
}

Scene& Scene::operator=(const Scene& other) {
    Scene tmp(other);
    Swap(tmp);
    return *this;
}

void Scene::Swap(Scene& other) {
    root_.swap(other.root_);
}

}  // namespace engine
