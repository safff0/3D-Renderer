#pragma once
#include "ascii_graphics.h"
#include "engine_fwd.h"

#include <string>
#include <system_error>

namespace app::donut {

class Application {
    template <typename T>
    using ConstReference = engine::ConstReference<T>;
    template <typename T>
    using Reference = engine::Reference<T>;
    using EmptyNode = engine::EmptyNode;
    using Scene = engine::Scene;

public:
    void Run();

    constexpr static int32_t kWidth = 80;
    constexpr static int32_t kHeight = 27;

private:
    void RotateDonut(Reference<EmptyNode> donut);

    Scene scene_;
    AsciiRenderer renderer_;
    AsciiDrawer drawer_;
};

}  // namespace app::donut
