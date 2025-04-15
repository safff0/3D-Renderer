#include "application.h"
#include "alias.h"
#include "ascii_graphics.h"
#include "engine_fwd.h"
#include "light.h"
#include "node.h"
#include "object3d.h"
#include "renderer.h"

#include <chrono>
#include <iostream>
#include <map>
#include <thread>

namespace app::donut {

using engine::Camera;
using engine::LightSource;
using engine::Object3D;

namespace {

const std::chrono::duration kSleepTime = std::chrono::milliseconds(30);
const float kRotationSpeed = 1.5;

const engine::Vector3 kCameraPosition = {0, 0, 4};

}  // namespace

void Application::Run() {
    auto root = scene_.GetRoot();
    auto camera =
        root.NewChild(Camera{Camera::Far{7}, Camera::Near{1.5}, Camera::FOV{50}}, kCameraPosition);
    auto donut = root.NewChild(Object3D::Torus(1, 0.5, 50));

    while (true) {
        RotateDonut(donut);
        AsciiRendererOutput frame =
            renderer_.Render(scene_, camera, engine::Width{kWidth}, engine::Height{kHeight});
        drawer_.Draw(frame);
    }
}

void Application::RotateDonut(Reference<EmptyNode> donut) {
    donut.SetRotationX(kRotationSpeed);
    donut.SetRotationY(kRotationSpeed);
    donut.SetRotationZ(kRotationSpeed);
}

}  // namespace app::donut
