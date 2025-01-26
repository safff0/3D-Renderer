#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <array>
#include <initializer_list>
#include <vector>

namespace engine {

using MathType = float;
using Vector2 = glm::vec<2, MathType>;
using Vector3 = glm::vec<3, MathType>;
using Vector4 = glm::vec<4, MathType>;
using Matrix3 = glm::mat<3, 3, MathType>;
using Matrix4 = glm::mat<4, 4, MathType>;

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform);
Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform);

class Polygon {
public:
    using IndexType = size_t;
    static constexpr size_t kVerticiesCount = 3;

    Polygon() = delete;
    Polygon(const std::array<Vector3, kVerticiesCount>& init);
    Polygon(std::initializer_list<Vector3> init);

    Polygon ApplyProjection(const Matrix4& proj, MathType near, MathType far) const;
    void ApplyProjectionInplace(const Matrix4& proj, MathType near, MathType far);

    const std::array<Vector3, kVerticiesCount>& GetVerticies() const;
    const std::array<MathType, kVerticiesCount>& GetZBuffer() const;
    std::array<Vector3, kVerticiesCount>& GetVerticies();
    std::array<MathType, kVerticiesCount>& GetZBuffer();

private:
    MathType GetZBuffer(MathType z, MathType near, MathType far) const;

    std::array<Vector3, kVerticiesCount> data_;
    std::array<MathType, kVerticiesCount> z_buffer_;
};

}  // namespace engine