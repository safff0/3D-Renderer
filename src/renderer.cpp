#include "renderer.h"
#include "alias.h"
#include "camera.h"
#include "geometry.h"
#include "node.h"
#include "object3d.h"
#include "reference.h"
#include "scene.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>

#include <cassert>
#include <iostream>
#include <vector>

namespace engine {

namespace details {

namespace {

RendererOutput InitOutput(Index width, Index height) {
    RendererOutput result;
    result.width = width;
    result.height = height;
    result.z_buffer.assign(height, std::vector<Real>(width, 1));
    return result;
}

}  // namespace

class RendererImpl {
public:
    using Real = Real;
    using SizeType = Renderer::SizeType;

    RendererOutput Render(const Scene& scene, ConstReference<Camera> camera, Width width,
                          Height height, Real stretch_aspect) const {
        std::vector<Polygon> polygons;
        FindPolygons(scene.GetRoot(), polygons);
        PerspectiveProjection(polygons, camera, GetAspectRatio(width, height));
        Normalize(polygons, width, height, stretch_aspect);
        return BuildOutput(width, height, polygons);
    }

private:
    Real GetAspectRatio(Width width, Height height) const {
        return static_cast<Real>(width) / height;
    }

    RendererOutput BuildOutput(Width width, Height height,
                               const std::vector<Polygon>& polygons) const {
        RendererOutput result = InitOutput(width, height);
        for (const Polygon& poly : polygons) {
            DrawPolygon(result, poly);
        }
        return result;
    }

    void Normalize(std::vector<Polygon>& polygons, Width width, Height height,
                   Real stretch_aspect) const {
        for (Polygon& poly : polygons) {
            ToScreenSpace(poly, width, height, stretch_aspect);
            Reorder(poly);
        }
    }

    void DrawPolygon(RendererOutput& result, const Polygon& p) const {
        const auto& verticies = p.GetVerticies();
        const auto& z_buf = p.GetZBuffer();
        Line2 l1{verticies[0], verticies[2]};
        Line2 l2{verticies[0], verticies[1]};
        Line2 l3{verticies[1], verticies[2]};
        for (SizeType x = verticies[0].x; x < verticies[1].x; ++x) {
            SizeType y1 = l1.GetY(x);
            SizeType y2 = l2.GetY(x);
            if (y1 > y2) {
                std::swap(y1, y2);
            }
            for (SizeType y = y1 - 1; y < y2 + 1; ++y) {
                if (z_buf[0] < result.z_buffer[x][y]) {
                    result.z_buffer[x][y] = GetZProjectionCoordinate({x, y}, p);
                }
            }
        }
        for (SizeType x = verticies[1].x; x < verticies[2].x; ++x) {
            SizeType y1 = l1.GetY(x);
            SizeType y2 = l3.GetY(x);
            if (y1 > y2) {
                std::swap(y1, y2);
            }
            for (SizeType y = y1 - 1; y < y2 + 1; ++y) {
                if (z_buf[0] < result.z_buffer[x][y]) {
                    result.z_buffer[x][y] = GetZProjectionCoordinate({x, y}, p);
                }
            }
        }
        if (verticies[1].x == verticies[2].x) {
            for (SizeType y = verticies[1].y - 1; y <= verticies[2].y; ++y) {
                if (z_buf[0] < result.z_buffer[verticies[1].x][y]) {
                    result.z_buffer[verticies[1].x][y] =
                        GetZProjectionCoordinate({verticies[1].x, y}, p);
                }
            }
        }
    }

    void ToScreenSpace(Polygon& p, Width width, Height height, Real stretch_aspect) const {
        std::array<Vector3, Polygon::kVerticiesCount>& verticies = p.GetVerticies();
        for (SizeType i = 0; i < Polygon::kVerticiesCount; ++i) {
            Real new_x = (1.0f + verticies[i].y) / 2.0f * height;
            Real new_y = (1.0f - verticies[i].x) / 2.0f * width * stretch_aspect;
            verticies[i] = Vector3{std::round(new_x), std::round(new_y), verticies[i].z};
        }
    }

    void Reorder(Polygon& p) const {
        std::array<Vector3, Polygon::kVerticiesCount>& verticies = p.GetVerticies();
        std::array<Real, Polygon::kVerticiesCount>& z_buf = p.GetZBuffer();
        for (SizeType i = 0; i < Polygon::kVerticiesCount; ++i) {
            if (verticies[i].x < verticies[0].x) {
                std::swap(verticies[i], verticies[0]);
                std::swap(z_buf[i], z_buf[0]);
            }
        }
        if (verticies[1].x > verticies[2].x) {
            std::swap(verticies[1], verticies[2]);
            std::swap(z_buf[1], z_buf[2]);
        }
    }

    void FindPolygons(ConstReference<EmptyNode> node, std::vector<Polygon>& poly,
                      Matrix4 transform = Matrix4(1.0f)) const {
        transform = node.GetTransform() * transform;
        if (Is<Object3D>(node)) {
            auto mesh = As<Object3D>(node)->GetMesh();
            for (const Polygon& polygon : mesh) {
                poly.push_back(polygon.ApplyTransform(transform));
            }
        }
        for (SizeType i = 0; i < node.GetChildCount(); ++i) {
            FindPolygons(node.GetChild(i), poly, transform);
        }
    }

    void PerspectiveProjection(std::vector<Polygon>& poly, ConstReference<Camera> camera,
                               Real aspect) const {
        Matrix4 view_matrix = glm::translate<Real>(Matrix4(1.0), -1.0 * camera.GetPosition());
        Matrix4 projection_matrix = glm::perspective<Real>(glm::radians(camera->GetFOV()), aspect,
                                                           camera->GetNear(), camera->GetFar());
        ApplyTransform(poly, projection_matrix * view_matrix, camera->GetNear(), camera->GetFar());
    }

    void ApplyTransform(std::vector<Polygon>& poly, const Matrix4& transform, Real near,
                        Real far) const {
        for (Polygon& polygon : poly) {
            polygon.ApplyProjectionInplace(transform, near, far);
        }
    }
};

}  // namespace details

Renderer::Renderer() : impl_(std::make_unique<RendererImpl>()) {
}

Renderer::~Renderer() = default;

Renderer::Renderer(const Renderer& other) : impl_{std::make_unique<RendererImpl>(*other.impl_)} {
}

Renderer& Renderer::operator=(const Renderer& other) {
    Renderer tmp{other};
    Swap(tmp);
    return *this;
}

Renderer::Renderer(Renderer&& other) noexcept = default;

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    Swap(other);
    return *this;
}

RendererOutput Renderer::Render(const Scene& scene, ConstReference<Camera> camera, Width width,
                                Height height, Real stretch_aspect) const {
    return impl_->Render(scene, camera, width, height, stretch_aspect);
}

void Renderer::Swap(Renderer& other) {
    impl_.swap(other.impl_);
}

}  // namespace engine
