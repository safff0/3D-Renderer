# Documentation

## <span style="color: #2ecc71">Reference&lt;EngineType&gt;</span>
A core class for the engine. It works as a pointer to an object.

### Methods:
- Use `.method()` for object tree transformations (e.g. `.SetPosition()`, `.SetParent()`).
- Use `->method()` for object-specific methods (e.g. `->SetColor()` for `Object3D` and `->SetFOV()` for `Camera`).

This class also has a `const` variation, `ConstReference<EngineType>`, that prevents object modification.

---

## <span style="color: #2ecc71">Scene</span>

### <span style="color: #3498db">Reference&lt;EmptyNode&gt; GetRoot()</span>
Returns a reference to the root node of the scene tree. Returns `ConstReference<EmptyNode>` if the Scene is `const`.

---

## <span style="color: #2ecc71">Renderer</span>

### <span style="color: #3498db">RendererOutput Render(const Scene& scene, ConstReference&lt;Camera&gt; camera, Width width, Height height, Real stretch_aspect = 1.0)</span>
Creates a frame of the `scene`, viewed from the `camera`. Returns a `RendererOutput` with dimensions `(width, height)`.

- `stretch_aspect` can be used to horizontally scale the final image without modifying its size. For example, in ASCII rendering, your pixels (ASCII characters) aren't perfect squares, so you might want to stretch the final image without modifying the size.

---

## <span style="color: #2ecc71">EmptyNode</span>

A class that functions purely as a scene tree node. It can be useful for complex scene transformations and grouping objects. Any `Reference<T>` can be casted to `Reference<EmptyNode>`.

### <span style="color: #3498db">Reference&lt;EmptyNode&gt; GetParent()</span>
Returns a reference to the parent node in the tree. This method must not be called from the scene root node.

### <span style="color: #3498db">Reference&lt;EmptyNode&gt; GetChild(int index)</span>
Returns a reference to the node's child with number `index` (where 0 is the first added child, 1 is second, and so on...).

### <span style="color: #3498db">int GetChildCount()</span>
Returns the number of node's children.

### <span style="color: #3498db">void SetParent(Reference&lt;EmptyNode&gt; parent)</span>
Removes the current node's subtree from the tree and links it to `parent`.

### <span style="color: #3498db">void AddChild(Reference&lt;EmptyNode&gt; child)</span>
Removes `child` and its subtree from the tree and links it to this node.

### <span style="color: #3498db">Reference&lt;T&gt; NewChild(T&& object)</span>
Creates a new node, storing `object`, as a child of this node.

### <span style="color: #3498db">void DeleteSubtree()</span>
Deletes the node and its subtree from the scene.

### <span style="color: #3498db">void SetPosition(Vector3 shift)</span>
Changes the node's position by `shift` relative to its parent.

### <span style="color: #3498db">Vector3 GetPosition()</span>
Returns the node's global position.

### <span style="color: #3498db">Matrix4 GetTransform()</span>
Returns the node's transform relative to its parent.

### <span style="color: #3498db">Matrix4 GetReverseTransform()</span>
Returns the node's reverse transform relative to its parent.

### <span style="color: #3498db">Matrix4 GetGlobalTransform()</span>
Returns the node's global transform.

### <span style="color: #3498db">Matrix4 GetGlobalReverseTransform()</span>
Returns the node's global reverse transform.

### <span style="color: #3498db">void SetRotationOnAxis(double angle, Vector3 axis)</span>
Rotates the node on axis `axis` (in the node's current transform space) by `angle` (in degrees).

You can use `SetRotationX(angle)`, `SetRotationY(angle)`, or `SetRotationZ(angle)` for setting rotation on the X, Y, and Z axes, respectively.

### <span style="color: #3498db">friend bool Is&lt;T&gt;(Reference&lt;EmptyNode&gt; node)</span>
Checks if the node stores an object of type `T`.

### <span style="color: #3498db">friend Reference&lt;T&gt; As&lt;T&gt;(Reference&lt;EmptyNode&gt; node)</span>
Casts `EmptyNode` to type `T` if the stored object is of type `T`.

---

## <span style="color: #2ecc71">Camera</span>

The `Camera` class stores `FOV`, `Far`, and `Near` values, and provides `Set(value)` and `Get()` methods for them.

---

## <span style="color: #2ecc71">LightSource</span>

The `LightSource` class has two parameters: `color` and `energy` (the amount of light the source provides). It provides `Set(value)` and `Get()` methods for them.

---

## <span style="color: #2ecc71">Object3D</span>

### <span style="color: #3498db">std::vector<Polygon> GetMesh()</span>
Returns a list of polygons for the object.

### <span style="color: #3498db">void SetColor(Color color)</span>
Changes the object's color to `color`.

### <span style="color: #3498db">static Object3D Cube(double size)</span>
Creates a cube of size `size`.

### <span style="color: #3498db">static Object3D Sphere(double radius, int subdivide)</span>
Creates a sphere of radius `radius`, dividing the circle into `subdivide` polygons.

The default value for `subdivide` is `20`.

### <span style="color: #3498db">static Object3D Torus(double radius, double thickness, int subdivide)</span>
Creates a torus with an outer radius of `radius` and thickness `thickness`.

### <span style="color: #3498db">static Object3D Cylinder(double radius, double height, int subdivide)</span>
Creates a cylinder with a radius of `radius` and height `height`.

### <span style="color: #3498db">static Object3D Cone(double radius, double height, int subdivide)</span>
Creates a cone with a radius of `radius` and height `height`.

### <span style="color: #3498db">static Object3D FromFile(string path)</span>
Reads an `.obj` file from `path` and creates an object from it.
