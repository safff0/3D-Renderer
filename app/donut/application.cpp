#include "application.h"
#include "alias.h"
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
using engine::ConstReference;
using engine::EmptyNode;
using engine::LightSource;
using engine::Object3D;
using engine::Reference;
using engine::Renderer;
using engine::RendererOutput;
using engine::Scene;

namespace {
const static std::string kResetCommand = "\033[" + std::to_string(Application::kHeight) + "A";

const static std::chrono::duration kSleepTime = std::chrono::milliseconds(30);
const static float kRotationSpeed = 3;

const static std::string kAsciiColorPalette = "@$#*!=;:~-,. ";

}  // namespace

void AsciiRenderer::Draw(const Scene& scene, ConstReference<Camera> camera, engine::Width w,
                         engine::Height h) const {
    RendererOutput output = renderer_.Render(scene, camera, w, h, kAsciiStretchAspect);
    PrintOutput(output);
    ResetScreen();
}

void AsciiRenderer::ResetScreen() const {
    std::cout << kResetCommand;
}

char AsciiRenderer::GetShade(const RendererOutput& img, Index i, Index j) const {
    if (img.z_buffer[i][j] > 1 - engine::kEps) {
        // background
        return kAsciiColorPalette.back();
    }
    Index index = img.z_buffer[i][j] * (kAsciiColorPalette.size() - 2);
    index = std::min(index, kAsciiColorPalette.size() - 1);
    return kAsciiColorPalette[index];
}

void AsciiRenderer::PrintOutput(const RendererOutput& img) const {
    for (Index i = 0; i < img.height; ++i) {
        for (Index j = 0; j < img.width; ++j) {
            std::cout << GetShade(img, i, j);
        }
        std::cout << std::endl;
    }
}

void Application::Run() {
    auto root = scene_.GetRoot();
    auto camera = root.NewChild(Camera{Camera::Far{7}, Camera::Near{2}, Camera::FOV{50}});
    camera.SetPosition({0, 0, 4});
    auto donut = root.NewChild(Object3D::Torus(1, 0.5, 50));

    while (true) {
        Update(donut, camera);
    }
}

void Application::Update(Reference<EmptyNode> donut, Reference<Camera> camera) {
    donut.SetRotationX(kRotationSpeed);
    donut.SetRotationY(kRotationSpeed);
    donut.SetRotationZ(kRotationSpeed);
    ShowFrame(camera);
    std::this_thread::sleep_for(kSleepTime);
}

void Application::ShowFrame(Reference<Camera> camera) const {
    renderer_.Draw(scene_, camera, engine::Width{kWidth}, engine::Height{kHeight});
}

}  // namespace app::donut
