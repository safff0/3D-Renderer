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
    auto root = scene_.GetRoot();
    Reference<Camera> camera =
        root.NewChild<Camera>(Camera{Camera::Far{20.0f}, Camera::Near{1.0f}, Camera::FOV{30.0f}});
    root.NewChild<Object3D>(Object3D::Sphere(1));
    camera.SetPosition({2, 1, -12});

    // Draw Object's verticies in screen space
    auto result = renderer_.Render(scene_, camera, engine::Width{40}, engine::Height{20});
    Print(result);
}

}  // namespace app
