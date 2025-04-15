#pragma once
#include "alias.h"
#include "geometry.h"
#include "reference.h"

#include <memory>
#include <vector>

namespace engine {

using Width = Alias<Index, struct width_tag>;
using Height = Alias<Index, struct height_tag>;

template <typename T>
class Table {
public:
    Table() = default;

    Table(Index height, Index width, T default_value = T{}) : h_{height}, w_{width} {
        data_.assign(height * width, default_value);
    }

    Index Height() const {
        return h_;
    }

    Index Width() const {
        return w_;
    }

    T operator()(Index x, Index y) const {
        assert(x >= 0 && y >= 0 && x < h_ && y < w_ && "Table: Index out of range");
        return data_[x * w_ + y];
    }

    T& operator()(Index x, Index y) {
        assert(x >= 0 && y >= 0 && x < h_ && y < w_ && "Table: Index out of range");
        return data_[x * w_ + y];
    }

private:
    Index h_ = 0;
    Index w_ = 0;

    std::vector<T> data_;
};

struct RendererOutput {
    Table<Real> z_buffer;
    Table<Color> surface_color;
    Table<Color> visible_color;
    Table<Vector3> normal_map;
};

namespace details {
class RendererImpl;
}

enum RenderModeTag { Default, Optimize };

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
                          Height height, Real stretch_aspect = 1.0,
                          RenderModeTag mode = Default) const;

    void Swap(Renderer& other);

private:
    std::unique_ptr<RendererImpl> impl_;
};

}  // namespace engine
