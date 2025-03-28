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
#include <cmath>
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
    result.surface_color.assign(height, std::vector<Color>(width, colors::kColorBlack));
    return result;
}

struct ViewFrustum {
    explicit ViewFrustum(const Matrix4& pm)
        : left{pm[0][3] + pm[0][0], pm[1][3] + pm[1][0], pm[2][3] + pm[3][0], pm[3][3] + pm[3][0]},
          right{pm[0][3] - pm[0][0], pm[1][3] - pm[1][0], pm[2][3] - pm[3][0], pm[3][3] - pm[3][0]},
          bottom{pm[0][3] + pm[0][1], pm[1][3] + pm[1][1], pm[2][3] + pm[2][1],
                 pm[3][3] + pm[3][1]},
          top{pm[0][3] - pm[0][1], pm[1][3] - pm[1][1], pm[2][3] - pm[2][1], pm[3][3] - pm[3][1]},
          near{pm[0][2], pm[1][2], pm[2][2], pm[3][2]} {
    }

    Plane3 left;
    Plane3 right;
    Plane3 bottom;
    Plane3 top;
    Plane3 near;
};

bool PointInFrustum(Vector3 p, const ViewFrustum& vf) {
    return vf.bottom.EquationValue(p) < kEps && vf.top.EquationValue(p) < kEps &&
           vf.left.EquationValue(p) < kEps && vf.right.EquationValue(p) < kEps &&
           vf.near.EquationValue(p) < kEps;
}

bool PolygonInFrustum(const Polygon& poly, const ViewFrustum& vf) {
    return PointInFrustum(poly.GetVerticies()[0], vf) &&
           PointInFrustum(poly.GetVerticies()[1], vf) && PointInFrustum(poly.GetVerticies()[2], vf);
}

bool PolygonOutsideFrustum(const Polygon& poly, const ViewFrustum& vf) {
    return !PointInFrustum(poly.GetVerticies()[0], vf) &&
           !PointInFrustum(poly.GetVerticies()[1], vf) &&
           !PointInFrustum(poly.GetVerticies()[2], vf);
}

}  // namespace

class RendererImpl {
public:
    using Real = Real;
    using SizeType = Renderer::SizeType;

    RendererOutput Render(const Scene& scene, ConstReference<Camera> camera, Width width,
                          Height height, Real stretch_aspect) const {
        std::vector<Polygon> polygons;
        FindPolygons(scene.GetRoot(), polygons);  // Divide scene into polygons
        ViewProjection(polygons, scene, camera);  // Transform polygons into camera relative space
        Filter(polygons, camera);                 // Discard not visible polygons
        Clip(polygons, camera, GetAspectRatio(width, height, stretch_aspect));  // Clipping
        PerspectiveProjection(
            polygons, camera,
            GetAspectRatio(width, height, stretch_aspect));  // Perform perspective projection
        Normalize(polygons, width, height, stretch_aspect);  // Prepare polygons for rasterization
        return BuildOutput(width, height, polygons);         // Rasterize
    }

private:
    Real GetAspectRatio(Width width, Height height, Real stretch_aspect) const {
        return static_cast<Real>(width) / height / stretch_aspect;
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

    void UpdateResult(RendererOutput& result, Index x, Index y, const Polygon& p) const {
        if (GetZProjectionCoordinate({x, y}, p) < result.z_buffer[x][y]) {
            result.z_buffer[x][y] = GetZProjectionCoordinate({x, y}, p);
            result.surface_color[x][y] = p.GetColor();
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
            for (SizeType y = std::max(0, y1 - 1); y < std::min(y2 + 1, result.width); ++y) {
                UpdateResult(result, x, y, p);
            }
        }
        for (SizeType x = verticies[1].x; x < verticies[2].x; ++x) {
            SizeType y1 = l1.GetY(x);
            SizeType y2 = l3.GetY(x);
            if (y1 > y2) {
                std::swap(y1, y2);
            }
            for (SizeType y = std::max(0, y1 - 1); y < std::min(y2 + 1, result.width); ++y) {
                UpdateResult(result, x, y, p);
            }
        }
        if (verticies[1].x == verticies[2].x) {
            for (SizeType y = std::max(0.0, verticies[1].y - 1);
                 y <= std::min(verticies[2].y, result.width - 1.0); ++y) {
                UpdateResult(result, verticies[1].x, y, p);
            }
        }
    }

    void ToScreenSpace(Polygon& p, Width width, Height height, Real stretch_aspect) const {
        std::array<Vector3, Polygon::kVerticiesCount>& verticies = p.GetVerticies();
        for (SizeType i = 0; i < Polygon::kVerticiesCount; ++i) {
            Real new_x = (1.0f + verticies[i].y) / 2.0f * height;
            Real new_y = (1.0f - verticies[i].x) / 2.0f * width;
            verticies[i] = Vector3{std::round(new_x), std::round(new_y), verticies[i].z};
            verticies[i].x = std::max(0.0, std::min(verticies[i].x, height - 1.0));
            verticies[i].y = std::max(0.0, std::min(verticies[i].y, width - 1.0));
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

    void ViewProjection(std::vector<Polygon>& poly, const Scene& scene,
                        ConstReference<Camera> camera) const {
        ApplyTransform(poly, camera.GetGlobalReverseTransform());
    }

    void Filter(std::vector<Polygon>& poly, ConstReference<Camera> camera) const {
        std::vector<Polygon> new_poly;
        for (auto& p : poly) {
            if (glm::dot(p.GetNormal(), Vector3(0, 0, 1)) >= 0) {
                new_poly.push_back(p);
            }
        }
        poly = new_poly;
    }

    Matrix4 GetProjectionMatrix(ConstReference<Camera> camera, Real aspect) const {
        return glm::perspective<Real>(glm::radians(camera->GetFOV()), aspect, camera->GetNear(),
                                      camera->GetFar());
    }

    void Clip(std::vector<Polygon>& poly, ConstReference<Camera> camera, Real aspect) const {
        ViewFrustum vf(GetProjectionMatrix(camera, aspect));
        std::vector<Polygon> new_poly;
        for (const auto& p : poly) {
            if (PolygonInFrustum(p, vf)) {
                new_poly.push_back(p);
            } else if (!PolygonOutsideFrustum(p, vf)) {
                // TODO: For better clipping intersect polygon and frustum
                new_poly.push_back(p);
            }
        }
        poly = new_poly;
    }

    void PerspectiveProjection(std::vector<Polygon>& poly, ConstReference<Camera> camera,
                               Real aspect) const {
        Matrix4 projection_matrix = GetProjectionMatrix(camera, aspect);
        ApplyProjection(poly, projection_matrix, camera->GetNear(), camera->GetFar());
    }

    void ApplyTransform(std::vector<Polygon>& poly, const Matrix4& transform) const {
        for (Polygon& polygon : poly) {
            polygon.ApplyTransformInplace(transform);
        }
    }

    void ApplyProjection(std::vector<Polygon>& poly, const Matrix4& transform, Real near,
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
