#pragma once
#include "alias.h"
#include "color.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <array>
#include <initializer_list>
#include <vector>

namespace engine {

class Line2 {
public:
    using Type = Real;

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
    explicit Line3(Vector3 from, Vector3 to);

    Vector3 GetOrigin() const;
    Vector3 GetDirection() const;

private:
    Vector3 point_;
    Vector3 direction_;
};

class Polygon;

class Plane3 {
public:
    using Type = Real;

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

    Polygon(const Triangle3D& init);
    Polygon(std::initializer_list<Vector3> init);

    Polygon ApplyProjection(const Matrix4& proj, Real near, Real far) const;
    void ApplyProjectionInplace(const Matrix4& proj, Real near, Real far);

    Polygon ApplyTransform(const Matrix4& transform) const;
    void ApplyTransformInplace(const Matrix4& transform);

    const Triangle3D& GetVerticies() const;
    Triangle3D& GetVerticies();

    Vector3 GetNormal() const;
    void FlipNormal();

    void SetColor(Color new_color);
    Color GetColor() const;

private:
    static constexpr Color kDefaultColor = colors::kColorLightGray;

    Triangle3D data_;
    Color color_ = kDefaultColor;
};

Real CosineBetweenVectors(Vector3 v, Vector3 u);

Real GetZProjectionCoordinate(Vector2 point, const Polygon& poly);
Vector3 Centroid(const Triangle3D& poly);

Vector3 PointApplyTransform(Vector3 to, const Matrix3& transform);
Vector4 PointApplyTransform(Vector3 to, const Matrix4& transform);

bool PointInTriangle2D(Vector2 point, const Triangle2D& poly);

Vector3 Intersect(Plane3 plane, Line3 line);

Vector3 Mean(const std::vector<Vector3>& vectors);

}  // namespace engine
