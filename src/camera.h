#pragma once
#include "geometry.h"

namespace engine {

class Camera {
public:
    using Type = MathType;

    void SetFar(Type value);
    void SetNear(Type value);
    void SetFOV(Type value);

    Type GetFar() const;
    Type GetNear() const;
    Type GetFOV() const;

private:
    Type far_ = kDefaultFar;
    Type near_ = kDefaultNear;
    Type fov_ = kDefaultFOV;  // degrees

    static constexpr Camera::Type kDefaultFar = 100.0f;
    static constexpr Camera::Type kDefaultNear = 5.0f;
    static constexpr Camera::Type kDefaultFOV = 50.0f;  // degrees
};

}  // namespace engine
