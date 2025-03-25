#pragma once
#include <cstddef>
#include <glm/glm.hpp>

namespace engine {

using Real = double;
using Index = int32_t;

const static Real kInfinity = 1e9;
const static Real kEps = 1e-4;

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

using Width = Alias<Index, struct width_tag>;
using Height = Alias<Index, struct height_tag>;

}  // namespace engine
