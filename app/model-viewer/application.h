#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include "engine_fwd.h"
#include "node.h"

namespace app::mv {

class Application {
    using Index = uint32_t;

public:
    void Run(const std::string& model_path);

private:
    void BuildScene(const std::string& model_path);
    void ShowFrame();

    void HandleEvents();
    void OnClose(const sf::Event::Closed& ev);
    void OnResize(const sf::Event::Resized& ev);
    void OnMBPress(const sf::Event::MouseButtonPressed& ev);
    void OnMBRelease(const sf::Event::MouseButtonReleased& ev);
    void OnMouseMove(const sf::Event::MouseMoved& ev);
    void OnMouseScroll(const sf::Event::MouseWheelScrolled& ev);

    static constexpr Index kDefaultHeight = 600;
    static constexpr Index kDefaultWidth = 800;
    static constexpr engine::Vector3 kCameraShift = {0, 0, 5};
    static constexpr engine::Vector3 kAmbientLightPosition = {10, 10, 10};
    static constexpr engine::Real kPanSpeed = -200;
    static constexpr engine::Real kMoveSpeed = 5;
    static constexpr engine::Real kZoomSpeed = 0.3;

    Index width_ = kDefaultWidth;
    Index height_ = kDefaultHeight;
    sf::RenderWindow window_;

    engine::Renderer renderer_;
    engine::Scene scene_;
    engine::Reference<engine::EmptyNode> camera_pivot_;
    engine::Reference<engine::Camera> camera_;

    bool pan_ = false;
    sf::Vector2i last_mouse_pos_pan_;
    bool move_ = false;
    sf::Vector2i last_mouse_pos_move_;
};

};  // namespace app::mv
