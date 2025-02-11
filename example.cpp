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
    Reference<Camera> camera = my_scene.GetRoot().NewChild<Camera>(Camera{20.0f, 1.0f, 30.0f});
    my_scene.GetRoot().NewChild<Object3D>(Object3D::Cube(2));
    camera.SetPosition({2, 2, -12});

    // Draw Object's verticies in screen space
    auto result = my_renderer.Render(my_scene, camera, 40, 20);
    Print(result);
    return 0;
}
