#pragma once
#include "alias.h"
#include "geometry.h"

namespace engine {

class Camera {
public:
    using Type = Real;
    using Far = Alias<Type, struct far_tag>;
    using Near = Alias<Type, struct near_tag>;
    using FOV = Alias<Type, struct fov_tag>;

    Camera() = default;
    Camera(Far far, Near near, FOV fov);
    void SetFar(Type value);
    void SetNear(Type value);
    void SetFOV(Type value);

    Type GetFar() const;
    Type GetNear() const;
    Type GetFOV() const;

private:
    static constexpr Type kDefaultFar = 50.0f;
    static constexpr Type kDefaultNear = 1.0f;
    static constexpr Type kDefaultFOV = 50.0f;  // degrees

    Type far_ = kDefaultFar;
    Type near_ = kDefaultNear;
    Type fov_ = kDefaultFOV;  // degrees
};

}  // namespace engine
