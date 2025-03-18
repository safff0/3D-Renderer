#pragma once
#include "engine_fwd.h"

namespace app {

class Application {
public:
    void Run();

private:
    engine::Scene scene_;
    engine::Renderer renderer_;
};

}  // namespace app
