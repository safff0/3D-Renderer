#pragma once
#include "alias.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <array>
#include <initializer_list>
#include <vector>

namespace engine {

class Line2 {
public:
    using Type = Real;

    Line2() = delete;
    Line2(Vector2 from, Vector2 to);

    Type GetX(Type y) const;
    Type GetY(Type x) const;

    Type EquationValue(Vector2 point) const;

private:
    // ax + by + c = 0
    Type a_;
    Type b_;
    Type c_;
};

class Line3 {
public:
    Line3() = delete;
    explicit Line3(Vector3 from, Vector3 to);

private:
    Vector3 point_;
    Vector3 direction_;
};

class Polygon;

class Plane3 {
public:
    using Type = Real;

    Plane3() = delete;
    Plane3(Vector3 p1, Vector3 p2, Vector3 p3);
    Plane3(const Triangle3D& points);
    Plane3(const Polygon& points);
    explicit Plane3(Type a, Type b, Type c, Type d);

    Type GetX(Vector2 yz_proj) const;
    Type GetY(Vector2 xz_proj) const;
    Type GetZ(Vector2 xy_proj) const;

    Type EquationValue(Vector3 point) const;
    Vector4 GetCoefficients() const;

private:
    // ax + by + cz + d = 0
    Type a_;
    Type b_;
    Type c_;
    Type d_;
};

class Polygon {
public:
    using IndexType = Index;
    static constexpr IndexType kVerticiesCount = 3;

    Polygon() = delete;
    Polygon(const Triangle3D& init);
    Polygon(std::initializer_list<Vector3> init);

    Polygon ApplyProjection(const Matrix4& proj, Real near, Real far) const;
    void ApplyProjectionInplace(const Matrix4& proj, Real near, Real far);

    Polygon ApplyTransform(const Matrix4& transform) const;
    void ApplyTransformInplace(const Matrix4& transform);

    const Triangle3D& GetVerticies() const;
    const std::array<Real, kVerticiesCount>& GetZBuffer() const;
    Triangle3D& GetVerticies();
    std::array<Real, kVerticiesCount>& GetZBuffer();

    Vector3 GetNormal() const;

private:
    Triangle3D data_;
    std::array<Real, kVerticiesCount> z_buffer_;
};

Real GetZProjectionCoordinate(Vector2 point, const Polygon& poly);

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform);
Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform);

bool PointInTriangle2D(Vector2 point, const Triangle2D& poly);

bool IsDegenerate(const Triangle2D& poly);

Line3 IntersectPlanes(Plane3 p1, Plane3 p2);

}  // namespace engine
