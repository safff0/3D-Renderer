#pragma once
#include "geometry.h"

namespace engine {

class Object3D {
public:
    using Type = Real;
    using IndexType = Index;

    const std::vector<Polygon>& GetMesh() const;

    static Object3D Cube(Type size = kDefaultObjectSize);
    static Object3D Sphere(Type radius = kDefaultObjectSize,
                           IndexType subdivide = kDefaultSubdivision);
    static Object3D Torus(Type radius = kDefaultObjectSize, Type thickness = kDefaultTorusThickness,
                          IndexType subdivide = kDefaultSubdivision);
    static Object3D Cylinder(Type radius = kDefaultObjectSize, Type height = kDefaultObjectSize,
                             IndexType subdivide = kDefaultSubdivision);
    static Object3D Cone(Type radius = kDefaultObjectSize, Type height = kDefaultObjectSize,
                         IndexType subdivide = kDefaultSubdivision);

private:
    static constexpr Type kDefaultObjectSize = 1;
    static constexpr Type kDefaultSubdivision = 20;
    static constexpr Type kDefaultTorusThickness = 0.4;

    void AddPolygon(const Polygon& poly);
    void AddFace(const std::vector<Vector3>& face);

    std::vector<Polygon> mesh_ = {};
};

}  // namespace engine
