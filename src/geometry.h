#pragma once
#include "alias.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <array>
#include <initializer_list>
#include <vector>

namespace engine {

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform);
Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform);

class Polygon {
public:
    using IndexType = Index;
    static constexpr IndexType kVerticiesCount = 3;

    Polygon() = delete;
    Polygon(const std::array<Vector3, kVerticiesCount>& init);
    Polygon(std::initializer_list<Vector3> init);

    Polygon ApplyProjection(const Matrix4& proj, Real near, Real far) const;
    void ApplyProjectionInplace(const Matrix4& proj, Real near, Real far);

    const std::array<Vector3, kVerticiesCount>& GetVerticies() const;
    const std::array<Real, kVerticiesCount>& GetZBuffer() const;
    std::array<Vector3, kVerticiesCount>& GetVerticies();
    std::array<Real, kVerticiesCount>& GetZBuffer();

private:
    std::array<Vector3, kVerticiesCount> data_;
    std::array<Real, kVerticiesCount> z_buffer_;
};

}  // namespace engine
