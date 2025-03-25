#pragma once
#include "alias.h"
#include "camera.h"
#include "engine_fwd.h"
#include "node.h"
#include "object3d.h"
#include "reference.h"

#include <string>
#include <system_error>

namespace app::donut {

class AsciiRenderer {
public:
    void Draw(const engine::Scene& scene, engine::ConstReference<engine::Camera> camera,
              engine::Width w, engine::Height h) const;

private:
    constexpr static float kAsciiStretchAspect = 1.5;

    void ResetScreen() const;

    char GetShade(float z_value, float min_z, float max_z) const;

    void PrintOutput(const engine::RendererOutput& img) const;

    engine::Renderer renderer_;
};

class Application {

public:
    void Run();

    constexpr static int32_t kWidth = 80;
    constexpr static int32_t kHeight = 27;

private:
    void Update(engine::Reference<engine::EmptyNode> donut,
                engine::Reference<engine::Camera> camera);

    void ShowFrame(engine::Reference<engine::Camera> camera) const;

    engine::Scene scene_;
    AsciiRenderer renderer_;
};

}  // namespace app::donut
