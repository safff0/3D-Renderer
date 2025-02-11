#pragma once
#include <cstddef>
#include <glm/glm.hpp>

namespace engine {

using Real = float;
using Index = int32_t;

using Vector2 = glm::vec<2, Real>;
using Vector3 = glm::vec<3, Real>;
using Vector4 = glm::vec<4, Real>;
using Matrix3 = glm::mat<3, 3, Real>;
using Matrix4 = glm::mat<4, 4, Real>;

enum Height : Index {};
enum Width : Index {};

}  // namespace engine
