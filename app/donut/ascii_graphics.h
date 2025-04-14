#pragma once
#include "alias.h"
#include "engine_fwd.h"
#include "reference.h"
#include "renderer.h"
#include "scene.h"

namespace app::donut {

struct AsciiRendererOutput;

class AsciiRenderer {
    using Renderer = engine::Renderer;
    using Output = engine::RendererOutput;
    using Scene = engine::Scene;
    template <typename T>
    using ConstReference = engine::ConstReference<T>;
    using Camera = engine::Camera;
    using Width = engine::Width;
    using Height = engine::Height;

public:
    using Index = size_t;

    AsciiRendererOutput Render(const Scene& scene, ConstReference<Camera> camera, Width w,
                               Height h) const;

private:
    constexpr static float kAsciiStretchAspect = 1.5;

    char GetShade(const Output& img, Index i, Index j) const;
    AsciiRendererOutput BuildAsciiOutput(const Output& from) const;

    Renderer renderer_;
};

struct AsciiRendererOutput {
    engine::Table<char> pixel;
    engine::Table<engine::Real> z_buffer;
};

class AsciiDrawer {
public:
    using Index = AsciiRenderer::Index;

    void Draw(const AsciiRendererOutput& frame) const;

private:
    void ResetScreen(Index height) const;
};

}  // namespace app::donut
