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

const static std::string kConsoleColorRed = "\033[31m";
const static std::string kConsoleColorGreen = "\033[32m";
const static std::string kConsoleColorYellow = "\033[33m";
const static std::string kConsoleColorBlue = "\033[34m";
const static std::string kConsoleColorMagenta = "\033[35m";
const static std::string kConsoleColorCyan = "\033[36m";
const static std::string kConsoleColorWhite = "\033[37m";

struct ColorCmp {
    bool operator()(const engine::Color& v, const engine::Color& u) const {
        for (size_t i = 0; i < 3; ++i) {
            if (v[i] != u[i]) {
                return v[i] < u[i];
            }
        }
        return false;
    }
};

const static std::map<engine::Color, std::string, ColorCmp> kColorMap{
    {engine::colors::kColorRed, kConsoleColorRed},
    {engine::colors::kColorGreen, kConsoleColorGreen},
    {engine::colors::kColorYellow, kConsoleColorYellow},
    {engine::colors::kColorBlue, kConsoleColorBlue},
    {engine::colors::kColorPurple, kConsoleColorMagenta},
    {engine::colors::kColorPink, kConsoleColorMagenta},
    {engine::colors::kColorCyan, kConsoleColorCyan},
    {engine::colors::kColorWhite, kConsoleColorWhite}};

engine::Real ColorDistance(engine::Color a, engine::Color b) {
    return glm::length(static_cast<engine::Vector3>(a) - static_cast<engine::Vector3>(b));
}

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

char AsciiRenderer::GetShade(const RendererOutput& img, Index i, Index j, float min_z,
                             float max_z) const {
    if (img.z_buffer[i][j] > 1 - engine::kEps) {
        // background
        return kAsciiColorPalette.back();
    }
    Index index = (img.z_buffer[i][j] - min_z) / (max_z - min_z) * (kAsciiColorPalette.size() - 2);
    index = std::min(index, kAsciiColorPalette.size() - 1);
    return kAsciiColorPalette[index];
}

std::string AsciiRenderer::GetColor(const RendererOutput& img, Index i, Index j) const {
    engine::Color c = img.visible_color[i][j];
    engine::Real distance = ColorDistance(engine::colors::kColorWhite, c);
    std::string result = kConsoleColorWhite;
    for (const auto& [engine_color, console_color] : kColorMap) {
        if (ColorDistance(engine_color, c) < distance) {
            distance = ColorDistance(engine_color, c);
            result = console_color;
        }
    }
    return result;
}

void AsciiRenderer::PrintOutput(const RendererOutput& img) const {
    engine::Real max_z = -1;
    engine::Real min_z = 1;
    for (Index i = 0; i < img.height; ++i) {
        for (Index j = 0; j < img.width; ++j) {
            if (img.z_buffer[i][j] < 1) {
                max_z = std::max(max_z, img.z_buffer[i][j]);
                min_z = std::min(min_z, img.z_buffer[i][j]);
            }
        }
    }
    for (Index i = 0; i < img.height; ++i) {
        for (Index j = 0; j < img.width; ++j) {
            std::cout << GetColor(img, i, j) << GetShade(img, i, j, min_z, max_z);
        }
        std::cout << std::endl;
    }
}

void Application::Run() {
    auto root = scene_.GetRoot();
    auto camera = root.NewChild(Camera{Camera::Far{20}, Camera::Near{0.1}, Camera::FOV{50}});
    camera.SetPosition({0, 0, -4});
    auto donut = root.NewChild(Object3D::Torus(1, 0.5, 50));
    auto lights = root.NewChild(LightSource());
    lights.SetPosition({0, 0, -5});

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
