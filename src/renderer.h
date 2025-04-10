#pragma once
#include "alias.h"
#include "geometry.h"
#include "reference.h"

#include <memory>
#include <vector>

namespace engine {

struct RendererOutput {
    Index width;
    Index height;
    std::vector<std::vector<Real>> z_buffer;
    std::vector<std::vector<Color>> surface_color;
    std::vector<std::vector<Color>> visible_color;
    std::vector<std::vector<Vector3>> normal_;
};

namespace details {
class RendererImpl;
}

class Scene;

class Renderer {
    using RendererImpl = details::RendererImpl;

public:
    using SizeType = Index;

    Renderer();
    ~Renderer();
    Renderer(const Renderer& other);
    Renderer& operator=(const Renderer& other);
    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    RendererOutput Render(const Scene& scene, ConstReference<Camera> camera, Width width,
                          Height height, Real stretch_aspect = 1.0) const;

    void Swap(Renderer& other);

private:
    std::unique_ptr<RendererImpl> impl_;
};

}  // namespace engine
