#include "application.h"
#include "alias.h"
#include "camera.h"
#include "engine_fwd.h"
#include "light.h"
#include "node.h"
#include "object3d.h"
#include "renderer.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>

#include <glm/ext/matrix_transform.hpp>

namespace app::mv {

namespace {

const engine::Real kMaxZoom = 10;
const engine::Real kMinZoom = 140;
const std::string kAppName = "Model Viewer";

std::vector<uint8_t> SfFrameFromEngineOutput(const engine::RendererOutput& output) {
    engine::Index size = 4 * output.visible_color.Height() * output.visible_color.Width();
    std::vector<uint8_t> pixels(size);
    engine::Index index = 0;
    for (engine::Index r = 0; r < output.visible_color.Height(); ++r) {
        for (engine::Index c = 0; c < output.visible_color.Width(); ++c) {
            pixels[index] = output.visible_color(r, c)[0];
            pixels[index + 1] = output.visible_color(r, c)[1];
            pixels[index + 2] = output.visible_color(r, c)[2];
            pixels[index + 3] = 255;
            index += 4;
        }
    }
    return pixels;
}

void OnClose(const sf::Event::Closed& ev, ApplicationState& state) {
    state.running = false;
}

void OnMBPress(const sf::Event::MouseButtonPressed& ev, ApplicationState& state) {
    if (ev.button == sf::Mouse::Button::Left) {
        state.rotate = RotationState::Rotate;
    }
    if (ev.button == sf::Mouse::Button::Right) {
        state.move = MovementState::Move;
    }
}

void OnMBRelease(const sf::Event::MouseButtonReleased& ev, ApplicationState& state) {
    if (ev.button == sf::Mouse::Button::Left) {
        state.rotate = RotationState::Stop;
    }
    if (ev.button == sf::Mouse::Button::Right) {
        state.move = MovementState::Stop;
    }
}

void OnMouseMove(const sf::Event::MouseMoved& ev, ApplicationState& state) {
    state.mouse_pos = ev.position;
}

void OnMouseScroll(const sf::Event::MouseWheelScrolled& ev, ApplicationState& state) {
    if (ev.wheel == sf::Mouse::Wheel::Vertical) {
        state.zoom = ev.delta;
    }
}

bool PointInWindow(sf::Vector2i pos, const sf::RenderWindow& window) {
    return pos.x > 0 && pos.x < window.getSize().x && pos.y > 0 && pos.y < window.getSize().y;
}

sf::Texture TextureFromRenderOutput(const engine::RendererOutput& fr) {
    sf::Texture frame(sf::Vector2u{fr.visible_color.Width(), fr.visible_color.Height()});
    frame.update(SfFrameFromEngineOutput(fr).data());
    return frame;
}

}  // namespace

Application::Application(const FilePath& model_path) : model_path_{model_path} {
    assert(std::filesystem::exists(model_path_) && "Application: .obj file does not exist");
    ConstructScene(model_path_);
    window_.create(sf::VideoMode(sf::Vector2u{kDefaultWidth, kDefaultHeight}), kAppName,
                   sf::Style::Titlebar | sf::Style::Close);
}

void Application::Run() {
    while (window_.isOpen()) {
        ApplicationState new_state = HandleInput();
        if (!new_state.running) {
            window_.close();
            break;
        }
        UpdateScene(new_state);
        DisplayFrame(BuildFrame());
        ChangeState(new_state);
    }
}

void Application::ConstructScene(const FilePath& model_path) {
    auto root = scene_.GetRoot();
    root.NewChild(engine::Object3D::FromFile(model_path));
    camera_pivot_ = root.NewChild(EmptyNode{});
    auto ambient_light = camera_pivot_.NewChild(engine::LightSource{0.5}, kAmbientLightPosition);
    camera_ = camera_pivot_.NewChild(Camera{}, kCameraShift);
    auto camera_light = camera_pivot_.NewChild(engine::LightSource{0.5}, kCameraShift);
}

sf::Texture Application::BuildFrame() {
    auto window_size = window_.getSize();
    auto fr = renderer_.Render(scene_, camera_, engine::Width{window_size.x},
                               engine::Height{window_size.y});
    return TextureFromRenderOutput(fr);
}

void Application::DisplayFrame(const sf::Texture& frame) {
    window_.draw(sf::Sprite{frame});
    window_.display();
}

ApplicationState Application::HandleInput() {
    ApplicationState state;
    while (const std::optional event = window_.pollEvent()) {
        if (const auto* ev = event->getIf<sf::Event::Closed>()) {
            OnClose(*ev, state);
        } else if (const auto* ev = event->getIf<sf::Event::MouseButtonPressed>()) {
            OnMBPress(*ev, state);
        } else if (const auto* ev = event->getIf<sf::Event::MouseButtonReleased>()) {
            OnMBRelease(*ev, state);
        } else if (const auto* ev = event->getIf<sf::Event::MouseMoved>()) {
            OnMouseMove(*ev, state);
        } else if (const auto* ev = event->getIf<sf::Event::MouseWheelScrolled>()) {
            OnMouseScroll(*ev, state);
        }
    }
    return state;
}

void Application::UpdateScene(const ApplicationState& new_state) {
    if (last_state_.rotate == RotationState::Rotate &&
        PointInWindow(new_state.mouse_pos, window_)) {
        engine::Real rotate_y =
            kPanSpeed * (new_state.mouse_pos.x - last_state_.mouse_pos.x) / window_.getSize().x;
        engine::Real rotate_x =
            kPanSpeed * (new_state.mouse_pos.y - last_state_.mouse_pos.y) / window_.getSize().y;
        camera_pivot_.SetRotationY(rotate_y);
        camera_pivot_.SetRotationX(rotate_x);
    }
    if (last_state_.move == MovementState::Move && PointInWindow(new_state.mouse_pos, window_)) {
        engine::Real move_x =
            -kMoveSpeed * (new_state.mouse_pos.x - last_state_.mouse_pos.x) / window_.getSize().x;
        engine::Real move_y =
            kMoveSpeed * (new_state.mouse_pos.y - last_state_.mouse_pos.y) / window_.getSize().y;
        camera_pivot_.SetPosition({move_x, move_y, 0});
    }
    engine::Real fov = camera_->GetFOV();
    camera_->SetFOV(std::min(kMinZoom, std::max(fov - kZoomSpeed * new_state.zoom, kMaxZoom)));
}

void Application::ChangeState(const ApplicationState& new_state) {
    if (PointInWindow(new_state.mouse_pos, window_)) {
        last_state_.mouse_pos = new_state.mouse_pos;
    }
    last_state_.zoom = new_state.zoom;
    if (new_state.rotate != RotationState::Keep) {
        last_state_.rotate = new_state.rotate;
    }
    if (new_state.move != MovementState::Keep) {
        last_state_.move = new_state.move;
    }
}

}  // namespace app::mv
