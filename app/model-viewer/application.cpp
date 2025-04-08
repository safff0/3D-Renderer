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
#include <SFML/Window/Mouse.hpp>

#include <glm/ext/matrix_transform.hpp>

namespace app::mv {

namespace {

std::vector<uint8_t> SfFrameFromEngineOutput(const engine::RendererOutput& output) {
    engine::Index size = 4 * output.height * output.width;
    std::vector<uint8_t> pixels(size);
    engine::Index index = 0;
    for (engine::Index r = 0; r < output.height; ++r) {
        for (engine::Index c = 0; c < output.width; ++c) {
            pixels[index] = output.visible_color[r][c][0];
            pixels[index + 1] = output.visible_color[r][c][1];
            pixels[index + 2] = output.visible_color[r][c][2];
            pixels[index + 3] = 255;
            index += 4;
        }
    }
    return pixels;
}

const static engine::Matrix4 kRotationMatrixX =
    glm::rotate(engine::Matrix4(1.0), glm::radians(90.0), {1.0, 0.0, 0.0});

const static engine::Real kMaxZoom = 30;
const static engine::Real kMinZoom = 140;

}  // namespace

void Application::Run(const std::string& model_path) {
    BuildScene(model_path);
    window_.create(sf::VideoMode(sf::Vector2u{kDefaultWidth, kDefaultHeight}), "Model viewer");
    while (window_.isOpen()) {
        HandleEvents();
        ShowFrame();
    }
}

void Application::BuildScene(const std::string& model_path) {
    auto root = scene_.GetRoot();
    root.NewChild(engine::Object3D::FromFile(model_path));
    camera_pivot_ = root.NewChild(engine::EmptyNode{});
    auto ambient_light =
        camera_pivot_.NewChild(engine::LightSource{0.5, engine::colors::kColorPink});
    ambient_light.SetPosition(kAmbientLightPosition);
    camera_ = camera_pivot_.NewChild(engine::Camera{});
    camera_.SetPosition(kCameraShift);
    auto camera_light = camera_pivot_.NewChild(engine::LightSource{0.5});
    camera_light.SetPosition(kCameraShift);
}

void Application::ShowFrame() {
    auto result = renderer_.Render(scene_, camera_, engine::Width{width_}, engine::Height{height_});
    sf::Texture frame(sf::Vector2u{width_, height_});
    frame.update(SfFrameFromEngineOutput(result).data());
    window_.draw(sf::Sprite{frame});
    window_.display();
}

void Application::HandleEvents() {
    while (const std::optional event = window_.pollEvent()) {
        if (const auto* ev = event->getIf<sf::Event::Closed>()) {
            OnClose(*ev);
        } else if (const auto* ev = event->getIf<sf::Event::Resized>()) {
            OnResize(*ev);
        } else if (const auto* ev = event->getIf<sf::Event::MouseButtonPressed>()) {
            OnMBPress(*ev);
        } else if (const auto* ev = event->getIf<sf::Event::MouseButtonReleased>()) {
            OnMBRelease(*ev);
        } else if (const auto* ev = event->getIf<sf::Event::MouseMoved>()) {
            OnMouseMove(*ev);
        } else if (const auto* ev = event->getIf<sf::Event::MouseWheelScrolled>()) {
            OnMouseScroll(*ev);
        }
    }
}

void Application::OnClose(const sf::Event::Closed& ev) {
    window_.close();
}

void Application::OnResize(const sf::Event::Resized& ev) {
    width_ = ev.size.x;
    height_ = ev.size.y;
}

void Application::OnMBPress(const sf::Event::MouseButtonPressed& ev) {
    if (ev.button == sf::Mouse::Button::Left) {
        pan_ = true;
        last_mouse_pos_pan_ = sf::Mouse::getPosition(window_);
    } else if (ev.button == sf::Mouse::Button::Right) {
        move_ = true;
        last_mouse_pos_move_ = sf::Mouse::getPosition(window_);
    }
}

void Application::OnMBRelease(const sf::Event::MouseButtonReleased& ev) {
    if (ev.button == sf::Mouse::Button::Left) {
        pan_ = false;
    } else if (ev.button == sf::Mouse::Button::Right) {
        move_ = false;
    }
}

void Application::OnMouseMove(const sf::Event::MouseMoved& ev) {
    if (pan_) {
        engine::Real rotate_y = kPanSpeed * (ev.position.x - last_mouse_pos_pan_.x) / width_;
        engine::Real rotate_x = kPanSpeed * (ev.position.y - last_mouse_pos_pan_.y) / height_;
        camera_pivot_.SetRotationY(rotate_y);
        camera_pivot_.SetRotationX(rotate_x);
        last_mouse_pos_pan_ = sf::Mouse::getPosition(window_);
    }
    if (move_) {
        engine::Real move_x = -kMoveSpeed * (ev.position.x - last_mouse_pos_move_.x) / width_;
        engine::Real move_y = kMoveSpeed * (ev.position.y - last_mouse_pos_move_.y) / height_;
        camera_pivot_.SetPosition({move_x, move_y, 0});
        last_mouse_pos_move_ = sf::Mouse::getPosition(window_);
    }
}

void Application::OnMouseScroll(const sf::Event::MouseWheelScrolled& ev) {
    engine::Real fov = camera_->GetFOV();
    if (ev.wheel == sf::Mouse::Wheel::Vertical) {
        camera_->SetFOV(std::min(kMinZoom, std::max(fov - kZoomSpeed * ev.delta, kMaxZoom)));
    }
}

}  // namespace app::mv
