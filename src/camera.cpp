#include "camera.h"

#include <cassert>

namespace engine {

namespace {}  // namespace

void Camera::SetFar(Type value) {
    far_ = value;
    assert(far_ > near_ && "Camera: Camera far is less than near");
}

void Camera::SetNear(Type value) {
    near_ = value;
    assert(far_ > near_ && "Camera: Camera far is less than near");
    assert(near_ >= 0 && "Camera: Camera near is negative");
}

void Camera::SetFOV(Type value) {
    fov_ = value;
    assert(fov_ >= 0 && "Camera: Camera fov is negative");
}

Camera::Type Camera::GetFar() const {
    return far_;
}

Camera::Type Camera::GetNear() const {
    return near_;
}

Camera::Type Camera::GetFOV() const {
    return fov_;
}

}  // namespace engine
