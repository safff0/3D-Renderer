#include "scene.h"
#include "node.h"
#include "reference.h"

namespace engine {

ConstReference<EmptyNode> Scene::GetRoot() const {
    return ConstReference<EmptyNode>{root_.get()};
}

}  // namespace engine
