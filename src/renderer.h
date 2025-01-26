#pragma once
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
    std::vector<std::vector<MathType>> z_buffer;
};

class Renderer {
public:
    static const Renderer* GetSingleton();

    RendererOutput Render(const Scene& scene, ConstReference<Camera> camera, size_t width) const;

private:
    Renderer();
    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer(Renderer&& other) = delete;
    Renderer& operator=(Renderer&& other) = delete;

    std::unique_ptr<details::RendererImpl> impl_;
};

}  // namespace engine