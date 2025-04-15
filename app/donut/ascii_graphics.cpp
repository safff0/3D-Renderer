#include "ascii_graphics.h"
#include "engine_fwd.h"
#include "renderer.h"

#include <iostream>

namespace app::donut {

namespace {

const std::string kAsciiColorPalette = "@$#*!=;:~-,. ";

}  // namespace

char AsciiRenderer::GetShade(const Output& img, Index i, Index j) const {
    if (img.z_buffer(i, j) > 1 - engine::kEps) {
        // background
        return kAsciiColorPalette.back();
    }
    Index index = img.z_buffer(i, j) * (kAsciiColorPalette.size() - 2);
    index = std::min(index, kAsciiColorPalette.size() - 1);
    return kAsciiColorPalette[index];
}

AsciiRendererOutput AsciiRenderer::BuildAsciiOutput(const Output& output) const {
    AsciiRendererOutput result{.z_buffer = output.z_buffer};
    result.pixel = engine::Table(output.visible_color.Height(), output.visible_color.Width(), ' ');
    for (Index i = 0; i < result.pixel.Height(); ++i) {
        for (Index j = 0; j < result.pixel.Width(); ++j) {
            result.pixel(i, j) = GetShade(output, i, j);
        }
    }
    return result;
}

AsciiRendererOutput AsciiRenderer::Render(const Scene& scene, ConstReference<Camera> camera,
                                          Width w, Height h) const {
    Output output = renderer_.Render(scene, camera, w, h, kAsciiStretchAspect, engine::Optimize);
    return BuildAsciiOutput(output);
}

void AsciiDrawer::Draw(const AsciiRendererOutput& frame) const {
    for (Index i = 0; i < frame.pixel.Height(); ++i) {
        for (Index j = 0; j < frame.pixel.Width(); ++j) {
            std::cout << frame.pixel(i, j);
        }
        std::cout << std::endl;
    }
    ResetScreen(frame.pixel.Height());
}

void AsciiDrawer::ResetScreen(Index height) const {
    std::string reset_command = "\033[" + std::to_string(height) + "A";
    std::cout << reset_command;
}

}  // namespace app::donut
