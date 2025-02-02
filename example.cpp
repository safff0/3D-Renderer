#include "node.h"
#include "object3d.h"
#include "reference.h"
#include "renderer.h"
#include "scene.h"

#include <iostream>

using namespace engine;

int main() {
    Scene my_scene;
    Reference<Camera> camera = my_scene.NewNode<Camera>();
    my_scene.NewNode<Object3D>(Object3D::Cube(2));
    camera.SetPosition({2, 2, -12});
    camera->SetFOV(30);
    camera->SetFar(20);
    camera->SetNear(1);

    // Draw Object's verticies in screen space
    auto result = Renderer::GetSingleton()->Render(my_scene, camera, 40, 20);
    for (size_t i = 0; i < result.data.size(); ++i) {
        for (size_t j = 0; j < result.data[i].size(); ++j) {
            std::cout << result.data[i][j];
        }
        std::cout << std::endl;
    }
    return 0;
}
