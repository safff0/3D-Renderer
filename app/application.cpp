#include "application.h"
#include "engine_fwd.h"

namespace app {

using engine::Camera;
using engine::ConstReference;
using engine::EmptyNode;
using engine::Object3D;
using engine::Reference;
using engine::Renderer;
using engine::Scene;

void Application::Run() {
    // Usage Example
    Reference<EmptyNode> root = scene_.GetRoot();
    Reference<Camera> camera = root.NewChild<Camera>(Camera{20.0f, 1.0f, 30.0f});
    root.NewChild<Object3D>(Object3D::Cube(2));
    camera.SetPosition({2, 2, -12});

    // Draw Object's verticies in screen space
    auto result = renderer_.Render(scene_, camera, engine::Width{40}, engine::Height{20});
    Print(result);
}

}  // namespace app
