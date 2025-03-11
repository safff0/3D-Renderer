#pragma once
#include "engine_fwd.h"

namespace app {

class Application {
public:
    void Run();

private:
    Scene scene_;
    Renderer renderer_;
};

}  // namespace app
