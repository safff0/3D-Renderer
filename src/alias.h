#pragma once
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>

namespace engine {

using Real = double;
using Index = uint32_t;
using Signed = int32_t;
using FilePath = std::filesystem::path;

constexpr inline Real kInfinity = 1e9;
constexpr inline Real kEps = 1e-4;

using Vector2 = glm::vec<2, Real>;
using Vector3 = glm::vec<3, Real>;
using Vector4 = glm::vec<4, Real>;
using Matrix3 = glm::mat<3, 3, Real>;
using Matrix4 = glm::mat<4, 4, Real>;

using Triangle3D = std::array<Vector3, 3>;
using Triangle2D = std::array<Vector2, 3>;

template <class T, class Tag>
struct Alias {
    explicit Alias(T value) : value_(value) {
    }

    operator T() const {
        return value_;
    }

private:
    T value_;
};

}  // namespace engine
