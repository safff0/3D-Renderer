#include "renderer.h"
#include "alias.h"
#include "camera.h"
#include "color.h"
#include "geometry.h"
#include "light.h"
#include "node.h"
#include "object3d.h"
#include "reference.h"
#include "scene.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include <cassert>
#include <cmath>
#include <vector>

namespace engine {

namespace details {

namespace {

constexpr Real kAmbientLightEnergy = 0.2;
constexpr Real kD = 1;
constexpr Real kS = 1;
constexpr Real kShininess = 3.0;

RendererOutput InitOutput(Index width, Index height) {
    RendererOutput result;
    result.z_buffer = Table<Real>{height, width, 1};
    result.surface_color = Table<Color>{height, width, engine::colors::kColorBlack};
    result.visible_color = Table<Color>{height, width, colors::kColorBlack};
    result.normal_map = Table<Vector3>{height, width, {0, 0, 1}};
    return result;
}

struct ViewFrustum {
    explicit ViewFrustum(const Matrix4& pm)
        : left{pm[0][3] + pm[0][0], pm[1][3] + pm[1][0], pm[2][3] + pm[2][0], pm[3][3] + pm[3][0]},
          right{pm[0][3] - pm[0][0], pm[1][3] - pm[1][0], pm[2][3] - pm[2][0], pm[3][3] - pm[3][0]},
          bottom{pm[0][3] + pm[0][1], pm[1][3] + pm[1][1], pm[2][3] + pm[2][1],
                 pm[3][3] + pm[3][1]},
          top{pm[0][3] - pm[0][1], pm[1][3] - pm[1][1], pm[2][3] - pm[2][1], pm[3][3] - pm[3][1]},
          near{pm[0][3] + pm[0][2], pm[1][3] + pm[1][2], pm[2][3] + pm[2][2], pm[3][3] + pm[3][2]},
          far{pm[0][3] - pm[0][2], pm[1][3] - pm[1][2], pm[2][3] - pm[2][2], pm[3][3] - pm[3][2]} {
    }

    Plane3 left;
    Plane3 right;
    Plane3 bottom;
    Plane3 top;
    Plane3 near;
    Plane3 far;
};

std::vector<Plane3> ClippingPlanes(const ViewFrustum& vf) {
    return {vf.left, vf.top, vf.right, vf.bottom, vf.near, vf.far};
}

enum class PolygonInFrustumState { Outside, Inside, NeedsClipping };

bool PointInFrustum(Vector3 p, const ViewFrustum& vf) {
    return vf.bottom.EquationValue(p) > -kEps && vf.top.EquationValue(p) > -kEps &&
           vf.left.EquationValue(p) > -kEps && vf.right.EquationValue(p) > -kEps &&
           vf.near.EquationValue(p) > -kEps;
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

PolygonInFrustumState GetPolygonFrustumState(const Polygon& poly, const ViewFrustum& vf) {
    if (PolygonInFrustum(poly, vf)) {
        return PolygonInFrustumState::Inside;
    }
    if (PolygonOutsideFrustum(poly, vf)) {
        return PolygonInFrustumState::Outside;
    }
    return PolygonInFrustumState::NeedsClipping;
}

struct LightInfo {
    Vector3 position;
    LightSource info;
};

}  // namespace

class RendererImpl {
public:
    using Real = Real;
    using SizeType = Renderer::SizeType;

    RendererOutput Render(const Scene& scene, ConstReference<Camera> camera, Width width,
                          Height height, Real stretch_aspect, RenderModeTag mode) const {
        std::vector<Polygon> polygons;
        std::vector<LightInfo> lights;
        FindKeyObjects(scene.GetRoot(), polygons,
                       lights);  // Divide scene into polygons and light sources
        ViewProjection(polygons, lights, scene,
                       camera);    // Transform polygons into camera relative space
        Filter(polygons, camera);  // Discard not visible polygons
        Clip(polygons, camera, GetAspectRatio(width, height, stretch_aspect));  // Clipping
        auto visible_colors = GetVisibleColors(polygons, lights, GetShadingAlgo(mode));
        PerspectiveProjection(
            polygons, camera,
            GetAspectRatio(width, height, stretch_aspect));  // Perform perspective projection
        Normalize(polygons, width, height, stretch_aspect);  // Prepare polygons for rasterization
        return BuildOutput(width, height, polygons, visible_colors, mode);  // Rasterize
    }

private:
    using ShadingAlgo = void (RendererImpl::*)(const Polygon&, const std::vector<LightInfo>&,
                                               std::vector<std::vector<Color>>&) const;

    Real GetAspectRatio(Width width, Height height, Real stretch_aspect) const {
        return static_cast<Real>(width) / height / stretch_aspect;
    }

    ShadingAlgo GetShadingAlgo(RenderModeTag mode) const {
        switch (mode) {
            case Optimize:
                return &RendererImpl::DoNoShading;
            case Default:
                return &RendererImpl::DoShading;
        }
        return &RendererImpl::DoNoShading;
    }

    RendererOutput BuildOutput(Width width, Height height, const std::vector<Polygon>& polygons,
                               const std::vector<std::vector<Color>>& colors,
                               RenderModeTag mode) const {
        RendererOutput result = InitOutput(width, height);
        for (Index i = 0; i < polygons.size(); ++i) {
            DrawPolygon(result, polygons[i], colors[i], mode);
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

    inline Color CalculateColor(Index x, Index y, const Polygon& p, const std::vector<Color>& color,
                                RenderModeTag mode) const {
        if (mode == Optimize) {
            return color[0];
        }
        Vector3 result{0, 0, 0};
        Real sum = 0;
        for (Index i = 0; i < Polygon::kVerticiesCount; ++i) {
            assert(colors::IsCorrect(Vector3(color[i])));
            Real len = glm::length(Vector2(x, y) - Vector2(p.GetVerticies()[i]));
            result += Vector3(color[i]) / (len + 1);
            sum += (1.0 / (len + 1));
        }
        result /= sum;
        return result;
    }

    void UpdateResult(RendererOutput& result, Index x, Index y, const Polygon& p,
                      const std::vector<Color>& color, RenderModeTag mode) const {
        if (GetZProjectionCoordinate({x, y}, p) < result.z_buffer(x, y)) {
            result.z_buffer(x, y) = GetZProjectionCoordinate({x, y}, p);
            result.surface_color(x, y) = p.GetColor();
            result.visible_color(x, y) = CalculateColor(x, y, p, color, mode);
            result.normal_map(x, y) = -1.0 * p.GetNormal();
        }
    }

    void DrawPolygon(RendererOutput& result, const Polygon& p, const std::vector<Color>& color,
                     RenderModeTag mode) const {
        const auto& verticies = p.GetVerticies();
        Line2 l1{verticies[0], verticies[2]};
        Line2 l2{verticies[0], verticies[1]};
        Line2 l3{verticies[1], verticies[2]};
        for (SizeType x = verticies[0].x; x < verticies[1].x; ++x) {
            Real y1 = l1.GetY(x);
            Real y2 = l2.GetY(x);
            if (y1 > y2) {
                std::swap(y1, y2);
            }
            for (SizeType y = std::max(0.0, y1);
                 y < std::min(y2, static_cast<Real>(result.z_buffer.Width())); ++y) {
                UpdateResult(result, x, y, p, color, mode);
            }
        }
        for (SizeType x = verticies[1].x; x < verticies[2].x; ++x) {
            Real y1 = l1.GetY(x);
            Real y2 = l3.GetY(x);
            if (y1 > y2) {
                std::swap(y1, y2);
            }
            for (SizeType y = std::max(0.0, y1);
                 y < std::min(y2, static_cast<Real>(result.z_buffer.Width())); ++y) {
                UpdateResult(result, x, y, p, color, mode);
            }
        }
        if (verticies[1].x == verticies[2].x) {
            for (SizeType y = std::max(0.0, verticies[1].y);
                 y < std::min(verticies[2].y, static_cast<Real>(result.z_buffer.Width())); ++y) {
                UpdateResult(result, verticies[1].x, y, p, color, mode);
            }
        }
    }

    void ToScreenSpace(Polygon& p, Width width, Height height, Real stretch_aspect) const {
        std::array<Vector3, Polygon::kVerticiesCount>& verticies = p.GetVerticies();
        for (SizeType i = 0; i < Polygon::kVerticiesCount; ++i) {
            Real new_x = (1.0f - verticies[i].y) / 2.0f * height;
            Real new_y = (1.0f + verticies[i].x) / 2.0f * width;
            verticies[i] = Vector3{std::round(new_x), std::round(new_y), verticies[i].z};
            verticies[i].x = std::max(0.0, std::min(verticies[i].x, height - 1.0));
            verticies[i].y = std::max(0.0, std::min(verticies[i].y, width - 1.0));
        }
    }

    void Reorder(Polygon& p) const {
        std::array<Vector3, Polygon::kVerticiesCount>& verticies = p.GetVerticies();
        for (SizeType i = 0; i < Polygon::kVerticiesCount; ++i) {
            if (verticies[i].x < verticies[0].x) {
                std::swap(verticies[i], verticies[0]);
            }
        }
        if (verticies[1].x > verticies[2].x) {
            std::swap(verticies[1], verticies[2]);
        }
    }

    void FindKeyObjects(ConstReference<EmptyNode> node, std::vector<Polygon>& poly,
                        std::vector<LightInfo>& lights, Matrix4 transform = Matrix4(1.0f)) const {
        transform = node.GetLocalTransform() * transform;
        if (Is<Object3D>(node)) {
            auto mesh = As<Object3D>(node)->GetMesh();
            for (const Polygon& polygon : mesh) {
                poly.push_back(polygon.ApplyTransform(transform));
            }
        } else if (Is<LightSource>(node)) {
            lights.push_back({.position = transform[3], .info = *As<LightSource>(node)});
        }
        for (SizeType i = 0; i < node.GetChildCount(); ++i) {
            FindKeyObjects(node.GetChild(i), poly, lights, transform);
        }
    }

    void ViewProjection(std::vector<Polygon>& poly, std::vector<LightInfo>& lights,
                        const Scene& scene, ConstReference<Camera> camera) const {
        for (Polygon& polygon : poly) {
            polygon.ApplyTransformInplace(camera.GetGlobalReverseTransform());
        }
        for (LightInfo& light : lights) {
            light.position = PointApplyTransform(light.position, camera.GetLocalReverseTransform());
        }
    }

    void Filter(std::vector<Polygon>& poly, ConstReference<Camera> camera) const {
        std::vector<Polygon> new_poly;
        for (auto& p : poly) {
            if (glm::dot(p.GetNormal(), Centroid(p.GetVerticies())) > -kEps) {
                new_poly.push_back(p);
            }
        }
        poly = new_poly;
    }

    Matrix4 GetProjectionMatrix(ConstReference<Camera> camera, Real aspect) const {
        return glm::perspective<Real>(glm::radians(camera->GetFOV()), aspect, camera->GetNear(),
                                      camera->GetFar());
    }

    std::vector<Polygon> ClipPolyThroughPlane(const std::vector<Polygon>& buffer, Plane3 plane,
                                              std::vector<Polygon>& new_poly) const {
        std::vector<Polygon> new_buffer;
        for (const auto& pb : buffer) {
            Index outside = 0;
            Index id_outside = 0;
            Index id_inside = 0;
            std::vector<Index> ids;
            for (Index i = 0; i < Polygon::kVerticiesCount; ++i) {
                ids.push_back(i);
                if (plane.EquationValue(pb.GetVerticies()[i]) > -kEps) {
                    id_inside = i;
                } else {
                    ++outside;
                    id_outside = i;
                }
            }
            if (outside == 1) {
                ids.erase(std::find(ids.begin(), ids.end(), id_outside));
                auto p0 = Intersect(
                    plane, Line3{pb.GetVerticies()[ids[0]], pb.GetVerticies()[id_outside]});
                auto p1 = Intersect(
                    plane, Line3{pb.GetVerticies()[ids[1]], pb.GetVerticies()[id_outside]});
                new_buffer.push_back({p0, pb.GetVerticies()[ids[1]], pb.GetVerticies()[ids[0]]});
                new_buffer.push_back({p0, p1, pb.GetVerticies()[ids[1]]});
            } else if (outside == 2) {
                ids.erase(std::find(ids.begin(), ids.end(), id_inside));
                auto p0 = Intersect(plane,
                                    Line3{pb.GetVerticies()[ids[0]], pb.GetVerticies()[id_inside]});
                auto p1 = Intersect(plane,
                                    Line3{pb.GetVerticies()[ids[1]], pb.GetVerticies()[id_inside]});
                new_buffer.push_back({p0, p1, pb.GetVerticies()[id_inside]});
            } else {
                new_buffer.push_back(pb);
            }
        }
        return new_buffer;
    }

    void ClipPoly(const Polygon& p, const ViewFrustum& vf, std::vector<Polygon>& new_poly) const {
        std::vector<Polygon> buffer = {p};
        for (Plane3 plane : ClippingPlanes(vf)) {
            buffer = ClipPolyThroughPlane(buffer, plane, new_poly);
        }
        for (auto& pb : buffer) {
            if (glm::dot(pb.GetNormal(), p.GetNormal()) < 0) {
                pb.FlipNormal();
            }
        }
        new_poly.insert(new_poly.end(), buffer.begin(), buffer.end());
    }

    void Clip(std::vector<Polygon>& poly, ConstReference<Camera> camera, Real aspect) const {
        ViewFrustum vf(GetProjectionMatrix(camera, aspect));
        std::vector<Polygon> new_poly;
        for (const auto& p : poly) {
            switch (GetPolygonFrustumState(p, vf)) {
                case PolygonInFrustumState::Inside:
                    new_poly.push_back(p);
                    break;
                case PolygonInFrustumState::NeedsClipping:
                    ClipPoly(p, vf, new_poly);
                    break;
                case PolygonInFrustumState::Outside:
                    break;
            }
        }
        poly = new_poly;
    }

    Color GetPhongShading(Vector3 point, Color color, Vector3 p_normal,
                          const std::vector<LightInfo>& lights) const {
        Color res = static_cast<Vector3>(color) * kAmbientLightEnergy;
        for (const LightInfo& light : lights) {
            Vector3 light_direction = light.position - point;
            light_direction /= glm::length(light_direction);
            Vector3 normal = -p_normal;
            normal /= glm::length(normal);
            if (glm::dot(light_direction, normal) < 0) {
                continue;
            }
            Vector3 reflection = 2 * glm::dot(light_direction, normal) * normal - light_direction;
            reflection /= glm::length(reflection);
            Vector3 viewer_direction = -point;
            viewer_direction /= glm::length(viewer_direction);
            res += kD * glm::dot(light_direction, normal) * light.info.GetEnergy() *
                       Vector3(light.info.GetColor()) +
                   kS * std::pow(glm::dot(reflection, viewer_direction), kShininess) *
                       light.info.GetEnergy();
        }
        for (Index j = 0; j < 3; ++j) {
            res[j] = std::max(0u, std::min(res[j], 255u));
        }
        return res;
    }

    void DoNoShading(const Polygon& p, const std::vector<LightInfo>& lights,
                     std::vector<std::vector<Color>>& result) const {
        Vector3 point = Centroid(p.GetVerticies());
        result.push_back({GetPhongShading(point, p.GetColor(), p.GetNormal(), lights)});
    }

    void DoShading(const Polygon& p, const std::vector<LightInfo>& lights,
                   std::vector<std::vector<Color>>& result) const {
        std::vector<Color> arr(Polygon::kVerticiesCount);
        for (Index i = 0; i < Polygon::kVerticiesCount; ++i) {
            arr[i] = GetPhongShading(p.GetVerticies()[i], p.GetColor(), p.GetNormal(), lights);
        }
        result.push_back(arr);
    }

    std::vector<std::vector<Color>> GetVisibleColors(const std::vector<Polygon>& poly,
                                                     const std::vector<LightInfo>& lights,
                                                     ShadingAlgo shading) const {
        std::vector<std::vector<Color>> result;
        for (const auto& p : poly) {
            (this->*shading)(p, lights, result);
        }
        return result;
    }

    void PerspectiveProjection(std::vector<Polygon>& poly, ConstReference<Camera> camera,
                               Real aspect) const {
        Matrix4 projection_matrix = GetProjectionMatrix(camera, aspect);
        ApplyProjection(poly, projection_matrix, camera->GetNear(), camera->GetFar());
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

Renderer::Renderer(const Renderer& other)
    : impl_{other.impl_ != nullptr ? std::make_unique<RendererImpl>(*other.impl_) : nullptr} {
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
                                Height height, Real stretch_aspect, RenderModeTag mode) const {
    return impl_->Render(scene, camera, width, height, stretch_aspect, mode);
}

void Renderer::Swap(Renderer& other) {
    impl_.swap(other.impl_);
}

}  // namespace engine
