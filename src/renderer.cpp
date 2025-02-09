#include "renderer.h"
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
const std::string kColorScheme =
    "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,. ";
}  // namespace

class RendererImpl {
public:
    using Real = Real;
    using SizeType = Renderer::SizeType;

    RendererOutput Render(const Scene& scene, ConstReference<Camera> camera, SizeType width,
                          SizeType height) const {
        std::vector<Polygon> polygons;
        FindPolygons(scene.GetRoot(), polygons);
        PerspectiveProjection(polygons, camera, static_cast<Real>(width) / height);
        return BuildOutput(width, height, polygons);
    }

private:
    RendererOutput BuildOutput(SizeType width, SizeType height,
                               std::vector<Polygon>& polygons) const {
        RendererOutput result;
        result.data.assign(height, std::vector<char>(width, kColorScheme.back()));
        result.z_buffer.assign(height, std::vector<Real>(width, -1));
        for (Polygon& poly : polygons) {
            ToScreenSpace(poly, height, width);
            Reorder(poly);
            DrawPolygon(result, poly);
        }
        return result;
    }

    char GetShade(Real z) const {
        return kColorScheme[(kColorScheme.size() - 1) * z];
    }

    void DrawPolygon(RendererOutput& result, Polygon& p) const {
        auto& verticies = p.GetVerticies();
        auto& z_buf = p.GetZBuffer();
        for (SizeType i = 0; i < verticies.size(); ++i) {
            result.data[verticies[i].x][verticies[i].y] = GetShade(z_buf[i]);
        }
        // TODO: Draw faces
    }

    void ToScreenSpace(Polygon& p, SizeType height, SizeType width) const {
        std::array<Vector3, Polygon::kVerticiesCount>& verticies = p.GetVerticies();
        for (SizeType i = 0; i < Polygon::kVerticiesCount; ++i) {
            Real new_x = (1.0f + verticies[i].y) / 2.0f * height;
            Real new_y = (1.0f - verticies[i].x) / 2.0f * width;
            verticies[i] = Vector3{new_x, new_y, verticies[i].z};
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
        if (verticies[1].y > verticies[2].y) {
            std::swap(verticies[1], verticies[2]);
            std::swap(z_buf[1], z_buf[2]);
        }
    }

    void FindPolygons(ConstReference<EmptyNode> node, std::vector<Polygon>& poly) const {
        if (Is<Object3D>(node)) {
            auto mesh = static_cast<ConstReference<Object3D>>(node)->GetMesh();
            for (const Polygon& polygon : mesh) {
                poly.push_back(polygon);
            }
        }
        for (SizeType i = 0; i < node.GetChildCount(); ++i) {
            FindPolygons(node.GetChild(i), poly);
        }
    }

    void PerspectiveProjection(std::vector<Polygon>& poly, ConstReference<Camera> camera,
                               Real aspect) const {
        Matrix4 view_matrix = glm::translate<Real>(Matrix4(1.0), -1.0f * camera.GetPosition());
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

void Renderer::Swap(Renderer& other) {
    impl_.swap(other.impl_);
}

Renderer::Renderer(const Renderer& other) : impl_{std::make_unique<RendererImpl>(*other.impl_)} {
}

Renderer& Renderer::operator=(const Renderer& other) {
    Renderer tmp{other};
    Swap(tmp);
    return *this;
}

Renderer::Renderer(Renderer&& other) noexcept : impl_{other.impl_.release()} {
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    Swap(other);
    return *this;
}

RendererOutput Renderer::Render(const Scene& scene, ConstReference<Camera> camera, SizeType width,
                                SizeType height) const {
    return impl_->Render(scene, camera, width, height);
}

}  // namespace engine
