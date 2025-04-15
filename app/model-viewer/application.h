#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include <filesystem>
#include <string>

#include "camera.h"
#include "engine_fwd.h"
#include "node.h"
#include "renderer.h"
#include "scene.h"

namespace app::mv {

enum class RotationState { Keep, Rotate, Stop };
enum class MovementState { Keep, Move, Stop };

struct ApplicationState {
    RotationState rotate = RotationState::Keep;
    MovementState move = MovementState::Keep;
    sf::Vector2i mouse_pos = {-1, -1};
    engine::Real zoom = 0;
    bool running = true;
};

class Application {
    using Index = uint32_t;
    using FilePath = engine::FilePath;
    using Renderer = engine::Renderer;
    template <typename T>
    using Reference = engine::Reference<T>;
    using Camera = engine::Camera;
    using EmptyNode = engine::EmptyNode;
    using Scene = engine::Scene;

public:
    Application(const FilePath& model_path);

    void Run();

private:
    void ConstructScene(const FilePath& model_path);
    sf::Texture BuildFrame();
    void DisplayFrame(const sf::Texture& frame);

    ApplicationState HandleInput();

    void UpdateScene(const ApplicationState& new_state);
    void ChangeState(const ApplicationState& new_state);

    static constexpr Index kDefaultHeight = 600;
    static constexpr Index kDefaultWidth = 800;
    static constexpr engine::Vector3 kCameraShift = {0, 0, 5};
    static constexpr engine::Vector3 kAmbientLightPosition = {10, 10, 10};
    static constexpr engine::Real kPanSpeed = -200;
    static constexpr engine::Real kMoveSpeed = 5;
    static constexpr engine::Real kZoomSpeed = 0.3;

    sf::RenderWindow window_;

    Renderer renderer_;
    Scene scene_;
    Reference<EmptyNode> camera_pivot_;
    Reference<Camera> camera_;

    ApplicationState last_state_;

    FilePath model_path_;
};

};  // namespace app::mv
