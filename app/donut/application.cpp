#include "application.h"
#include "alias.h"
#include "engine_fwd.h"
#include "node.h"
#include "renderer.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace app::donut {

using engine::Camera;
using engine::ConstReference;
using engine::EmptyNode;
using engine::Object3D;
using engine::Reference;
using engine::Renderer;
using engine::RendererOutput;
using engine::Scene;

namespace {
const static std::string kResetCommand = "\033[" + std::to_string(Application::kHeight) + "A";

const static std::chrono::duration kSleepTime = std::chrono::milliseconds(30);
const static float kRotationSpeed = 1.2;

const static std::string kAsciiColorPalette = "@$#*!=;:~-,. ";

}  // namespace

void AsciiRenderer::Draw(Scene scene, ConstReference<Camera> camera, engine::Width w,
                         engine::Height h) const {
    RendererOutput output = renderer_.Render(scene, camera, w, h, kAsciiStretchAspect);
    PrintOutput(output);
    ResetScreen();
}

void AsciiRenderer::ResetScreen() const {
    std::cout << kResetCommand;
}

char AsciiRenderer::GetShade(float z_value, float min_z, float max_z) const {
    assert(z_value >= -1 && z_value <= 1 && "AsciiRender: z_buffer value is out of range");
    size_t index = (z_value - min_z) / (max_z - min_z) * (kAsciiColorPalette.size() - 2);
    if (index >= kAsciiColorPalette.size()) {
        index = kAsciiColorPalette.size() - 1;
    }
    return kAsciiColorPalette[index];
}

void AsciiRenderer::PrintOutput(const RendererOutput& img) const {
    float max_z = -1;
    float min_z = 1;
    for (size_t i = 0; i < img.height; ++i) {
        for (size_t j = 0; j < img.width; ++j) {
            if (img.z_buffer[i][j] < 1) {
                max_z = std::max(max_z, img.z_buffer[i][j]);
                min_z = std::min(min_z, img.z_buffer[i][j]);
            }
        }
    }
    for (size_t i = 0; i < img.height; ++i) {
        for (size_t j = 0; j < img.width; ++j) {
            std::cout << GetShade(img.z_buffer[i][j], min_z, max_z);
        }
        std::cout << std::endl;
    }
}

void Application::Run() {
    auto root = scene_.GetRoot();
    auto camera =
        root.NewChild<Camera>(Camera{Camera::Far{10}, Camera::Near{0.1}, Camera::FOV{50}});
    camera.SetPosition({0, 0, -4});
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
