#include "geometry.h"

#include <glm/vec3.hpp>

#include <array>
#include <initializer_list>
#include <iostream>

namespace engine {

namespace details {

static MathType GetZBuffer(MathType z, MathType near, MathType far) {
    return (far + near) / (far - near) - 2.0f / z * (-2.0 * far * near) / (far - near);
}

}  // namespace details

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform) {
    return transform * to;
}

Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform) {
    return transform * Vector4(to, 1.0f);
}

Polygon::Polygon(const std::array<Vector3, kVerticiesCount>& init) : data_(init) {
}

Polygon::Polygon(std::initializer_list<Vector3> init) {
    assert(init.size() == kVerticiesCount && "Geometry: Polygon must be initialized with 3 points");
    IndexType i = 0;
    for (auto& e : init) {
        data_[i] = e;
        ++i;
    }
}

Polygon Polygon::ApplyProjection(const Matrix4& transform, MathType near, MathType far) const {
    Polygon result(*this);
    result.ApplyProjectionInplace(transform, near, far);
    return result;
}

void Polygon::ApplyProjectionInplace(const Matrix4& transform, MathType near, MathType far) {
    for (IndexType i = 0; i < kVerticiesCount; ++i) {
        Vector4 tmp = PointApplyTransform(data_[i], transform);
        data_[i] = Vector3(tmp.x, tmp.y, tmp.z) / tmp.w;
        z_buffer_[i] = details::GetZBuffer(tmp.z, near, far);
    }
}

const std::array<Vector3, Polygon::kVerticiesCount>& Polygon::GetVerticies() const {
    return data_;
}

const std::array<MathType, Polygon::kVerticiesCount>& Polygon::GetZBuffer() const {
    return z_buffer_;
}

std::array<Vector3, Polygon::kVerticiesCount>& Polygon::GetVerticies() {
    return data_;
}

std::array<MathType, Polygon::kVerticiesCount>& Polygon::GetZBuffer() {
    return z_buffer_;
}

}  // namespace engine