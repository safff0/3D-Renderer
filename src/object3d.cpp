#include "object3d.h"
#include "geometry.h"

#include <vector>

namespace engine {

namespace {

Vector3 GetSphericalCoordinates(Object3D::Type radius, Object3D::Type phi, Object3D::Type psi) {
    return Vector3{radius * glm::sin(psi), radius * glm::cos(psi) * glm::cos(phi),
                   radius * glm::cos(psi) * glm::sin(phi)};
}

Vector3 GetToroidalCoordinates(Object3D::Type radius, Object3D::Type thickness, Object3D::Type phi,
                               Object3D::Type psi) {
    return Vector3{glm::cos(psi) * (radius + thickness * sin(phi)),
                   glm::sin(psi) * (radius + thickness * sin(phi)), thickness * glm::cos(phi)};
}

}  // namespace

const std::vector<Polygon>& Object3D::GetMesh() const {
    return mesh_;
}

Object3D Object3D::Cube(Type size) {
    Object3D result;
    for (auto& i : {-1, 1}) {
        Type h = (size + i * size) / 2.0;
        Type w = size / 2;
        result.AddFace({{w, h, w}, {-w, h, w}, {-w, h, -w}, {w, h, -w}});
        result.AddFace({{w, 0, i * w}, {-w, 0, i * w}, {-w, size, i * w}, {w, size, i * w}});
        result.AddFace({{i * w, 0, w}, {i * w, 0, -w}, {i * w, size, -w}, {i * w, size, w}});
    }
    return result;
}

Object3D Object3D::Sphere(Type radius, IndexType subdiv) {
    Object3D result;
    const Type pi = glm::pi<Type>();
    for (Type phi = 0; phi < 2 * pi; phi += pi / subdiv) {
        for (Type psi = -pi / 2; psi < pi / 2; psi += pi / subdiv) {
            Vector3 p1 = GetSphericalCoordinates(radius, phi, psi);
            Vector3 p2 = GetSphericalCoordinates(radius, phi + pi / subdiv, psi);
            Vector3 p3 = GetSphericalCoordinates(radius, phi + pi / subdiv, psi + pi / subdiv);
            Vector3 p4 = GetSphericalCoordinates(radius, phi, psi + pi / subdiv);
            result.AddFace({p1, p2, p3, p4});
        }
    }
    return result;
}

Object3D Object3D::Torus(Type radius, Type thickness, IndexType subdiv) {
    Object3D result;
    const Type pi = glm::pi<Type>();
    for (Type phi = 0; phi < 2 * pi; phi += 2 * pi / subdiv) {
        for (Type psi = 0; psi < 2 * pi; psi += 2 * pi / subdiv) {
            Vector3 p1 = GetToroidalCoordinates(radius, thickness, phi, psi);
            Vector3 p2 = GetToroidalCoordinates(radius, thickness, phi + 2 * pi / subdiv, psi);
            Vector3 p3 = GetToroidalCoordinates(radius, thickness, phi + 2 * pi / subdiv,
                                                psi + 2 * pi / subdiv);
            Vector3 p4 = GetToroidalCoordinates(radius, thickness, phi, psi + 2 * pi / subdiv);
            result.AddFace({p1, p2, p3, p4});
        }
    }
    return result;
}

Object3D Object3D::Cylinder(Type radius, Type height, IndexType subdiv) {
    Object3D result;
    const Type pi = glm::pi<Type>();
    for (Type phi = 0; phi < 2 * pi; phi += 2 * pi / subdiv) {
        Type x1 = radius * glm::cos(phi);
        Type z1 = radius * glm::sin(phi);
        Type x2 = radius * glm::cos(phi + 2 * pi / subdiv);
        Type z2 = radius * glm::sin(phi + 2 * pi / subdiv);
        result.AddFace({{x1, 0, z1}, {x1, height, z1}, {x2, height, z2}, {x2, 0, z2}});
        result.AddFace({{x1, 0, z1}, {0, 0, 0}, {x2, 0, z2}});
        result.AddFace({{x1, height, z1}, {0, height, 0}, {x2, height, z2}});
    }
    return result;
}

Object3D Object3D::Cone(Type radius, Type height, IndexType subdiv) {
    Object3D result;
    const Type pi = glm::pi<Type>();
    for (Type phi = 0; phi < 2 * pi; phi += 2 * pi / subdiv) {
        Type x1 = radius * glm::cos(phi);
        Type z1 = radius * glm::sin(phi);
        Type x2 = radius * glm::cos(phi + 2 * pi / subdiv);
        Type z2 = radius * glm::sin(phi + 2 * pi / subdiv);
        result.AddFace({{x1, 0, z1}, {0, height, 0}, {x2, 0, z2}});
        result.AddFace({{x1, 0, z1}, {0, 0, 0}, {x2, 0, z2}});
    }
    return result;
}

void Object3D::AddPolygon(const Polygon& poly) {
    mesh_.push_back(poly);
}

void Object3D::AddFace(const std::vector<Vector3>& face) {
    for (IndexType i = 1; i + 1 < face.size(); ++i) {
        AddPolygon(Polygon{face[0], face[i], face[i + 1]});
    }
}

}  // namespace engine
