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
                           IndexType subdivide = kDefaultSphereSubdiv);

private:
    static constexpr Type kDefaultObjectSize = 1;
    static constexpr Type kDefaultSphereSubdiv = 10;

    void AddPolygon(const Polygon& poly);
    void AddFace(const std::vector<Vector3>& face);

    std::vector<Polygon> mesh_ = {};
};

}  // namespace engine
