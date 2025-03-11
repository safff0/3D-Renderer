#include "geometry.h"

#include <glm/vec3.hpp>

#include <array>
#include <initializer_list>
#include <iostream>

namespace engine {

namespace {

Real GetZBufferCoordinate(Real z, Real near, Real far) {
    return (far + near) / (far - near) - 2.0f / z * (-2.0 * far * near) / (far - near);
}

}  // namespace

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform) {
    return transform * to;
}

Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform) {
    return transform * Vector4(to, 1.0f);
}

Line2::Line2(Vector2 from, Vector2 to)
    : a_{to.y - from.y}, b_{from.x - to.x}, c_{to.x * from.y - to.y * from.x} {
}

Line2::Type Line2::GetX(Type y) const {
    assert(std::abs(a_) > kEps && "Geometry: Line2 zero division");
    return -1.0 * (c_ + b_ * y) / a_;
}

Line2::Type Line2::GetY(Type x) const {
    assert(std::abs(b_) > kEps && "Geometry: Line2 zero division");
    return -1.0 * (c_ + a_ * x) / b_;
}

Line2::Type Line2::EquationValue(Vector2 point) const {
    return a_ * point.x + b_ * point.y + c_;
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

Polygon Polygon::ApplyProjection(const Matrix4& transform, Real near, Real far) const {
    Polygon result(*this);
    result.ApplyProjectionInplace(transform, near, far);
    return result;
}

void Polygon::ApplyProjectionInplace(const Matrix4& transform, Real near, Real far) {
    for (IndexType i = 0; i < kVerticiesCount; ++i) {
        Vector4 tmp = PointApplyTransform(data_[i], transform);
        data_[i] = Vector3(tmp.x, tmp.y, tmp.z) / tmp.w;
        z_buffer_[i] = GetZBufferCoordinate(tmp.z, near, far);
    }
}

Polygon Polygon::ApplyTransform(const Matrix4& transform) const {
    Polygon result(*this);
    result.ApplyTransformInplace(transform);
    return result;
}

void Polygon::ApplyTransformInplace(const Matrix4& transform) {
    for (IndexType i = 0; i < kVerticiesCount; ++i) {
        data_[i] = PointApplyTransform(data_[i], transform);
    }
}

const std::array<Vector3, Polygon::kVerticiesCount>& Polygon::GetVerticies() const {
    return data_;
}

const std::array<Real, Polygon::kVerticiesCount>& Polygon::GetZBuffer() const {
    return z_buffer_;
}

std::array<Vector3, Polygon::kVerticiesCount>& Polygon::GetVerticies() {
    return data_;
}

std::array<Real, Polygon::kVerticiesCount>& Polygon::GetZBuffer() {
    return z_buffer_;
}

}  // namespace engine
