#include "node.h"
#include "object3d.h"
#include "reference.h"
#include "renderer.h"
#include "scene.h"

#include <iostream>

using namespace engine;

int main() {
    Scene my_scene;
    Renderer my_renderer;
    Reference<Camera> camera = my_scene.NewNode<Camera>(20.0f, 1.0f, 30.0f);
    my_scene.NewNode<Object3D>(Object3D::Cube(2));
    camera.SetPosition({2, 2, -12});

    // Draw Object's verticies in screen space
    auto result = my_renderer.Render(my_scene, camera, 40, 20);
    for (size_t i = 0; i < result.data.size(); ++i) {
        for (size_t j = 0; j < result.data[i].size(); ++j) {
            std::cout << result.data[i][j];
        }
        std::cout << std::endl;
    }
    return 0;
}
