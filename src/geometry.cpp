#include "geometry.h"
#include "alias.h"

#include <glm/vec3.hpp>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>

namespace engine {

namespace {

Real GetZBufferCoordinate(Real z, Real near, Real far) {
    return (far + near) / (far - near) - 2.0f / z * (-2.0 * far * near) / (far - near);
}

}  // namespace

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

Line3::Line3(Vector3 from, Vector3 to) : point_{from}, direction_{to - from} {
}

Vector3 Line3::GetOrigin() const {
    return point_;
}

Vector3 Line3::GetDirection() const {
    return direction_;
}

Plane3::Plane3(Vector3 p1, Vector3 p2, Vector3 p3)
    : a_{(p2.y - p1.y) * (p3.z - p1.z) - (p2.z - p1.z) * (p3.y - p1.y)},
      b_{(p2.z - p1.z) * (p3.x - p1.x) - (p2.x - p1.x) * (p3.z - p1.z)},
      c_{(p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x)} {
    d_ = -(a_ * p1.x + b_ * p1.y + c_ * p1.z);
}

Plane3::Plane3(const Triangle3D& points) : Plane3{points[0], points[1], points[2]} {
}

Plane3::Plane3(const Polygon& points) : Plane3{points.GetVerticies()} {
}

Plane3::Plane3(Type a, Type b, Type c, Type d) : a_{a}, b_{b}, c_{c}, d_{d} {
}

Plane3::Type Plane3::GetX(Vector2 yz_proj) const {
    assert(std::abs(a_) > kEps && "Geometry: Plane3 zero division");
    return -(b_ * yz_proj[0] + c_ * yz_proj[1] + d_) / a_;
}

Plane3::Type Plane3::GetY(Vector2 xz_proj) const {
    assert(std::abs(b_) > kEps && "Geometry: Plane3 zero division");
    return -(a_ * xz_proj[0] + c_ * xz_proj[1] + d_) / b_;
}

Plane3::Type Plane3::GetZ(Vector2 xy_proj) const {
    assert(std::abs(c_) > kEps && "Geometry: Plane3 zero division");
    return -(a_ * xy_proj[0] + b_ * xy_proj[1] + d_) / c_;
}

Plane3::Type Plane3::EquationValue(Vector3 point) const {
    return a_ * point.x + b_ * point.y + c_ * point.z + d_;
}

Vector4 Plane3::GetCoefficients() const {
    return {a_, b_, c_, d_};
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

const Triangle3D& Polygon::GetVerticies() const {
    return data_;
}

Triangle3D& Polygon::GetVerticies() {
    return data_;
}

Vector3 Polygon::GetNormal() const {
    return Plane3(*this).GetCoefficients();
}

void Polygon::FlipNormal() {
    std::reverse(data_.begin(), data_.end());
}

void Polygon::SetColor(Color new_color) {
    assert(colors::IsCorrect(new_color) && "Polygon: Invalid color value");
    color_ = new_color;
}

Color Polygon::GetColor() const {
    return color_;
}

Real CosineBetweenVectors(Vector3 v, Vector3 u) {
    return glm::dot(v, u) / (glm::length(v) * glm::length(u));
}

Real GetZProjectionCoordinate(Vector2 point, const Polygon& poly) {
    auto plane = Plane3{poly};
    if (std::abs(plane.GetCoefficients()[2]) < kEps) {
        Real min_z = kInfinity;
        for (size_t i = 0; i < Polygon::kVerticiesCount; ++i) {
            min_z = std::min(min_z, poly.GetVerticies()[i].z);
        }
        return min_z;
    }
    return plane.GetZ(point);
}

Vector3 Centroid(const Triangle3D& poly) {
    return (poly[0] + poly[1] + poly[2]) / 3.0;
}

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform) {
    return transform * to;
}

Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform) {
    return transform * Vector4(to, 1.0f);
}

bool PointInTriangle2D(Vector2 point, const Triangle2D& poly) {
    return true;
}

Vector3 Intersect(Plane3 plane, Line3 line) {
    Real t = -plane.EquationValue(line.GetOrigin()) /
             (plane.EquationValue(line.GetDirection()) - plane.GetCoefficients()[3]);
    return line.GetOrigin() + t * line.GetDirection();
}

}  // namespace engine
