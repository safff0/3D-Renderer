#pragma once
#include "alias.h"
#include "geometry.h"
#include "reference.h"

#include <memory>
#include <vector>

namespace engine {

namespace details {
class RendererImpl;
}

class Scene;

struct RendererOutput {
    std::vector<std::vector<char>> data;
    std::vector<std::vector<Real>> z_buffer;
};

void Print(const RendererOutput& output);

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
                          Height height) const;

    void Swap(Renderer& other);

private:
    std::unique_ptr<RendererImpl> impl_;
};

}  // namespace engine
