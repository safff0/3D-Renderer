# 3D Renderer From Scratch

# Dependancies

- [GLM](https://github.com/g-truc/glm) (Used for matrix calculations)

# Features

- Clipping and Perspective projection

- Tree-like objects structure, which allows complicated transformations

- Dynamic lighting with colors

<p align="center">
  <img src="./docs/_media/light-demo.gif" alt="animated" />
</p>

# Examples

## Donut

(works on Unix systems only)

A recreation of famous "spinning donut in C", using this 3D renderer library.

### Build
```bash
mkdir build
cd build
cmake ..
make donut
./donut
```

<p align="center">
  <img src="./docs/_media/donut-demo.gif" alt="animated" />
</p>

# Creating custom 3D scenes
```C++
using namespace engine;

int main() {
    Scene my_scene;
    Renderer my_renderer;
    Reference<EmptyNode> root = my_scene.GetRoot();
    Reference<Camera> camera = root.NewChild(Camera());
    // Add cube with size = 1
    Reference<Object3D> cube = root.NewChild(Object3D::Cube(1));
    // Add light source as a child of cube
    Reference<LightSource> light = cube.NewChild(engine::LightSource());  
    // Shift light source relativly from cube
    light.SetPosition({-5, 0, 0});             
    light->SetColor(engine::colors::kColorOrange);

    // Get frame with info about pixel colors and z buffer
    RendererOutput result = 
        my_renderer.Render(my_scene, camera, width, height);
}
```
