#### Components Source Code Organization

This components library is a set of components; "component" is defined as a set of source files that:
- Has a well-defined purpose, scope, and feature set.
- Has well-defined dependencies, which are explicitly declared and are minimal.
Concisely put, a "component" can be thought of as a formal "sub-library".

To that end, each component's source code goes in its own separate directory so that the boundaries
between components are utterly clear; they can be encoded in not only the directory structure but
also the CMakeLists.txt include directories.  Each component can even have its own README.md file
(or even Doxygen doc set).  The advantages of this are:
- The build system enforces that the scope and dependencies of a component stay within that which
  has been declared by the human (thus not requiring humans to know about and voluntarily obey the
  scheme).
- Because the scope of each component is limited and its dependencies are clear, it is extremely
  easy to extract/use a single component or a small subset of components.  This is a huge plus for
  open source software, in that someone can lift a single component without also needing the
  overhead of some highly interdependent framework like Cinder.
- The overall architecture of the Components library becomes formally apparent.  The components
  that have no component-dependencies (but perhaps [system] library dependencies) could be
  considered the "lowest level" components (e.g. GLTexture2, Singleton), drawn at the bottom of
  a dependency graph.  Other components will build off of these, offering more power and
  higher-level interfaces (e.g. Application, GLTexture2FreeImage, Resource), and could therefore
  be drawn higher up on the dependency graph.  A developer-written application would use a
  selection of the higher-level components and therefore would occupy an even higher location
  in the dependency graph.  To put another way, the components at the bottom of the graph provide
  very low-level but granular control of the system.  Components at the top of the graph provide
  high-level but more-abstracted control of the system.  Therefore the dependency graph also
  functions as a spectrum in which to gauge the level-of-detail a component's interface could
  or should have.

#### Notes and TODOs

- There should be some cmake-based command where a set of components is specified, and then 
  those components are compiled into a single library, and the set of include directories for those
  components is returned to the caller.  This would allow a minimal, tailor-made "components"
  library to be generated (e.g. a library that only includes ResourceManager, Resource, Singleton,
  and SDLController).  This feature depends on and will exercise the above strict organization of
  components.
- Write a cmake function which looks at the set of components and generates a dot graph of their
  dependency graph -- for components and for [system] libraries.
- Add a "static std::string ResourceTypeName ()" method to ResourceLoader<T> which returns a
  std::string containing the name of T (e.g. ResourceLoader<GLShader>::ResourceTypeName() would
  return "GLShader").  This will be useful in ResourceManager messages.

#### 2014.08.15 - Proposal/Overview for Shader-based SVG rendering (Bezier-curve-only)

Proposed solution
- Two parts to the rendering
  1. Using piecewise polygon which the Bezier curves' control points define,
     invert-draw (flip bits in the stencil buffer) a triangle fan whose center
     point is an arbitrary point (can choose the centroid or median of the shape).
     This gives a piecewise-linear approximation of the smooth shape.
  2. Invert-draw each Bezier curve convex hull using the shader program which does
     the perfect curve computation.
- Define the "inside" of the curve to be to the left of the curve along its
  "drawing" direction (think of this as the direction the curve would go as you
  draw it with a pencil).

Algorithm:
  0. Shader program for drawing "perfect" Bezier curve quads:
    a. Resource: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch25.html
    b. Implementing this is not totally trivial, as the resource doesn't provide
       shader code for the cubic case.
  1. For each piecewise Bezier curve path, the following pre-processing will be done once:
    a. Ensure the path is closed by adding a straight Bezier curve between the
       endpoints if they are distinct.
    b. Determine the convex hull for each Bezier curve making up the path.  Store
       the vertices for a hull in counterclockwise winding order.  These convex
       hulls are attached at their "endpoints" which lie on the curve.
    c. Determine the piecewise linear path resulting from traversing the convex
       hulls in the curve direction and always to the left of the convex hull.
    d. Populate a vertex buffer with this piecewise linear path.  This defines
       a triangle fan that will be used to draw to the stencil buffer.
    e. Populate a vertex buffer with the convex hulls of the Bezier curves.
       Populate a texture coordinate buffer correspondingly to define curve
       space mappings for the convex hulls for the shader.  These points should
       probably be layed out in the buffer probably using GL_TRIANGLES.
  2. To draw a shape which consists of several piecewise Bezier curve paths:
    a. In order to draw a piecewise Bezier curve path, start with a cleared stencil
       buffer, and using an invert-draw operation (drawing a pixel means flipping
       that bit in the stencil buffer), draw the triangle fan defined in part (1.d).
    b. Load the cubic Bezier drawing shader program and invert-draw the Bezier
       curves defined in part (1.e).
    c. Using the stencil buffer to mask the color buffer drawing operation, draw
       the desired color over the entire stenciled region.  It would be sufficient
       to draw a quad over the bounding box.

Time estimate:
  TODO

Integration into the larger context (this would consume the "Bezier shape" component
and produce an SVGPrimitiveLoader component):
- Loading an SVG (via the header-only library "nanosvg" referenced below) should produce
  a hierarchy of PrimitiveBase nodes, which is exactly the scene graph defined by the SVG
  file.  There is a conceptually direct mapping from the SVG scene graph and its child-node
  transformations to our SceneGraphNode and its transformations.
- We could add a "name" property to PrimitiveBase, and in loading an SVG, extract the
  "name" (or "id"?) attribute from each SVG node.  Then add an accessor to PrimitiveBase
  which retrieves a named node by specifying the path through its ancestry tree.  For
  example, if there is a tree of Primitives

  SceneRoot
    ControlPanel
      VolumeControls
        VolumeKnob
        MuteButton
      VideoControls
        GammaCorrectionSlider
        FullScreenButton
    ...

  Then the application code could conveniently access any node in this tree through
  a call like the following:

    SceneRoot.Descendant("ControlPanel.VolumeControls") // returns the VolumeControls node
    SceneRoot.Descendant("ControlPanel.VolumeControls.VolumeKnob") // returns the VolumeKnob node
    SceneRoot.Descendant("") // returns SceneRoot (trivial case necessary for domain closure of the "Descendant" function)

Relevant technologies/links:
- https://github.com/memononen/nanosvg - Conversion of SVG to cubic Bezier curves
- http://http.developer.nvidia.com/GPUGems3/gpugems3_ch25.html - Lovely resource on GPU-based Bezier curve rendering
- https://code.google.com/p/poly2tri/ - Triangulation of polygon (related but unnecessary)

#### Design notes for primitives/GL refactor

GLShader now implements a dictionary for uniforms and attributes (name -> (location, type, size)).
GLShaderMatrices provides an interface for setting the expected matrix uniforms for shaders.
Material is now an interface for setting the parameters of a particular material fragment shader.

GLVertexBuffer
- Should somehow take a compile-time-specified list of vertex attributes and define
  a "vertex" structure which will contain data for rendering each vertex in a VBO.
- Should present a strongly-typed interface for adding these attributes to an intermediate
  representation of the vertex attributes before it's uploaded to GL.
- Should present a way to upload the intermediate vertex data to GL, and optionally
  clear the intermediate data.
- Should present a way to modify the intermediate vertex data.
- Should present a way to request to modify the uploaded data.
- Should have methods for indicating to GL that this VBO should be enabled (see PrimitiveGeometry::Draw),
  and that it should be disabled.

GLMesh<DIM>
- These are particular instances of GLVertexBuffer implemented for the following vertex attribs:
  * Position
  * Normal
  * Texture Coordinate
  * Color
  * Others?
  These should be dimension-specific
- This should probably present a few high-level methods for:
  * Defining a mesh
    ~ Specifying how the vertices are drawn (e.g. GL_TRIANGLES, GL_TRIANGLE_FAN, etc)
    ~ Uploading to GL
  * Modifying a mesh that has already been loaded
  * Drawing a mesh
  * Assigning UV coordinates
  * Generating particular geometric primitives (e.g. spheres, cylinders, etc)
  * Specifying which other attributes should be on a vertex.

