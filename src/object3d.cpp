#include "object3d.h"
#include "geometry.h"

#include <vector>

namespace engine {

namespace {

Vector3 GetSpericalCoordinates(Object3D::Type radius, Object3D::Type phi, Object3D::Type psi) {
    return Vector3{radius * glm::sin(psi), radius * glm::cos(psi) * glm::cos(phi),
                   radius * glm::cos(psi) * glm::sin(phi)};
}

}  // namespace

void Object3D::AddPolygon(const Polygon& poly) {
    mesh_.push_back(poly);
}

void Object3D::AddFace(const std::vector<Vector3>& face) {
    for (IndexType i = 1; i + 1 < face.size(); ++i) {
        AddPolygon(Polygon{face[0], face[i], face[i + 1]});
    }
}

const std::vector<Polygon>& Object3D::GetMesh() const {
    return mesh_;
}

Object3D Object3D::Cube(Type size) {
    Object3D result;
    for (auto& i : {-1, 1}) {
        Type h = (size + i * size) / 2.0;
        result.AddFace({{size / 2, h, size / 2},
                        {-size / 2, h, size / 2},
                        {-size / 2, h, -size / 2},
                        {size / 2, h, -size / 2}});
        for (auto& j : {-1, 1}) {
            result.AddFace({{i * size / 2, 0, j * size / 2},
                            {-i * size / 2, 0, j * size / 2},
                            {-i * size / 2, size, j * size / 2},
                            {i * size / 2, size, j * size / 2}});
        }
    }
    return result;
}

Object3D Object3D::Sphere(Type radius, IndexType subdiv) {
    Object3D result;
    const Type pi = glm::pi<Type>();
    for (Type phi = 0; phi < 2 * pi; phi += pi / subdiv) {
        for (Type psi = -pi / 2; psi < pi / 2; psi += pi / subdiv) {
            Vector3 p1 = GetSpericalCoordinates(radius, phi, psi);
            Vector3 p2 = GetSpericalCoordinates(radius, phi + pi / subdiv, psi);
            Vector3 p3 = GetSpericalCoordinates(radius, phi + pi / subdiv, psi + pi / subdiv);
            Vector3 p4 = GetSpericalCoordinates(radius, phi, psi + pi / subdiv);
            result.AddFace({p1, p2, p3, p4});
        }
    }
    return result;
}

}  // namespace engine
