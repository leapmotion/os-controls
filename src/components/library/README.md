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
- Look into using FBOs for headless rendering for purposes of GL unit testing, etc., because this
  may not require any windowing support, so e.g. headless GL unit tests wouldn't depend on
  SDLController or SFMLController.

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

GLVertexBuffer now implements the following:
- Should somehow take a compile-time-specified list of vertex attributes and define
  a "vertex" structure which will contain data for rendering each vertex in a VBO.
- Should present a strongly-typed interface for adding these attributes to an intermediate
  representation of the vertex attributes before it's uploaded to GL.
- Should present a way to upload the intermediate vertex data to GL, and optionally
  clear the intermediate data.
- Should present a way to modify the intermediate vertex data.
- Should have methods for indicating to GL that this VBO should be enabled (see PrimitiveGeometry::Draw),
  and that it should be disabled.

GLVertexBuffer probably needs more work on the following:
- Should present a way to request to modify the uploaded data.

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

#### 2014.09.23 - Notes for GL component

- Namespacing: Use Lm (Leap Motion) namespace for everything, and Gl for OpenGL stuff.
  e.g. Lm::Gl::Shader, Lm::Gl::Texture2.

- There are two detail-levels of GL components.  One is the granular, consisting of abstractions
  of separate OpenGL concepts.  The other is sort of "package level".  There are two disjoint package-level
  GL components, which I'll make up names for because they don't exist tangibly at this moment in time.
  * GLCoreComponent (better name needed) -- abstractions of OpenGL concepts; no external library
    requirements besides OpenGL and Glew.
  * GLResourceLoaders (better name needed) -- "extras" which use other external libraries (e.g. FreeImage)
  * GLEverything (better name needed) -- is a phony target that depends on GLCoreComponent and GLResourceLoaders.

  The granular components making up GLCoreComponent are:
  * GLCompatibility
    ~ gl_glext_glu.h -- uses Glew to include GL headers -- should rename to GLHeaders.h
    ~ GLError.h -- for flexible GL error checking -- could go in a different component.
  * GLTexture2 (depends on C++11)
    ~ GLTexture2Params -- persistent parameters for GLTexture2 (e.g. width, height, target, etc)
    ~ GLTexture2PixelData -- interface for specifying pixel data for loading/saving into/from GLTexture2
    ~ GLTexture2 -- Handle to OpenGL texture object
  * FrameBufferObject
    ~ FrameBufferObject
    ~ RenderBuffer
  * GLBuffer
    ~ GLBuffer -- abstracts the concept of an OpenGL buffer object
  * GLVertexBuffer (depends on C++11)
    ~ GLVertexAttribute -- abstracts the concept of an OpenGL vertex attribute
    ~ GLVertexBuffer -- abstracts the concept of an OpenGL vertex buffer object
  * GLController
    ~ GLController -- Was originally intended to be a frontend for non-redundantly controlling GL state,
                      but became a very lightweight set of "bookends" for rendering an OpenGL frame.
                      This class should either be deleted, or filled out into its original design.
  * GLMatrices (depends on EigenTypes)
    ~ ModelView -- Basically replaces the deprecated fixed-function pipeline regarding the GL_MODEL_VIEW matrix stack.
    ~ Projection -- Same, but for GL_PROJECTION
    I personally would like to tighten up the design on these, and ideally abstract away the dependence on Eigen.
  * GLShader (depends on C++11, ScopeGuard)
    ~ GLShader -- abstracts the concept of a GLSL shader program (vertex and fragment).  Do we want
                  to support geometry shaders?
    ~ GLShaderBindingScopeGuard -- an object which implements the "scope guard" for binding/unbinding shaders.
                                   This class is not strictly necessary, but is a convenience.
  * GLMaterial (depends on Color, EigenTypes)
    ~ GLMaterial -- provides a C++ interface for a particular fragment shader that we have written.  A lot of design
                    would should be done on this one, because it effectively implements a limited model of a 3D
                    material, but is being used in Primitives for what really should be 2D materials.  Thus some
                    notion of 2D and 3D materials should be designed.  Ideally we could devise an abstraction
                    for providing C++ interfaces to shaders in a more flexible way (to deal with the presence/lack
                    of particularly-named shader uniforms, etc).
    ~ GLShaderMatrices -- provides C++ interface for the matrix uniforms necessary to replace the fixed-function
                          pipeline deprecated in OpenGL 3.0 and removed in 3.1.  This is reasonably abstract
                          enough to not depend on hard-coded shader uniforms.

  Component dependencies:
  * EigenTypes (depends on Eigen).
  * C++11 (compile flags for C++11 support).
  * ScopeGuard (provides the "scope guard" pattern).
  * Color (color class(es) -- in a branch I have an expanded set of classes RGB<T> and RGBA<T> which define
    rich color operations).  Could/should be GLColor.

  There is a set of GL components that have additional library dependencies which I view as "extras", which make
  up the hypothetical "GLResourceLoaders" package-level component.  These are:
  * GLTexture2FreeImage -- provides a FreeImage-based loader for GLTexture2.  Depends on the FreeImage library.
  * GLTexture2Loader -- provides a ResourceLoader-based loader for GLTexture2 which uses GLTexture2FreeImage.
  * GLShaderLoader -- provides a ResourceLoader-based loader for GLShader from vertex/fragment shader source.

  The "loader" components use Resource, ResourceManager, and Singleton, which may not be dependencies we want to
  provide.  Ideally we could abstract dependence on these so that implementations that use Resource,
  ResourceManager, and Singleton could be implemented easily in particular applications that have made a positive
  choice to use them.  Other solutions, e.g. using Autowiring, should be just as easy to implement using the
  proposed abstractions.

  The existing GLShaderLoader could be broken up into 1) a class that reads the vertex/fragment shader source from
  disk and creates a GLShader and 2) the existing ResourceLoader<GLShader> which would then just be a
  ResourceLoader frontend for the loader in part 1.  Then the part 1 loader could be a part of the GLCoreComponents
  component.

- There are a few "non GL" component dependencies (EigenTypes, C++11, ScopeGuard, Color) that should be
  subsumed by or removed from the GL component, so that it has no other dependencies.  Notes on each:
  * EigenTypes -- with a careful design, templates could be used to abstract away explicit dependence on
    Eigen, and instead provide an option "Eigen add-on" which defines Eigen-specific template instantiations
    of the relevant types, along with the dependence on the external Eigen library.
  * C++11 -- this is a simple set of compiler flags, and doesn't constitute a nontrivial dependence.
  * ScopeGuard -- this is a single, templatized class that could easily be incorporated into the GL component.
  * Color -- this could (should) be made into a first-class GL component, as it provides very nice abstractions
    of many color concepts in OpenGL.

- I would like to abstract away the use of Eigen using carefully designed templates.  Then there can be typedefs
  for the "Eigen versions" of each these which transparently replace the existing Eigen-based classes.  I
  suggest using a _t suffix for template classes.  This has the convenient feature that within some C++ scope,
  one can make a typedef for a particular template instantiation like the following:

    template <typename T> SomeClass_t;

    ...

    {
      typedef SomeClass_t<float> SomeClass;
      ...
    }

- GLColor component (code exists on components/color branch)
  * Implements a rich set of color component operations in various underlying types that could be 
  * Currently there are RGB<T> and RGBA<T> color types which support the following types for T:
    ~ uint8_t (range is [0,2^8-1])
    ~ uint16_t (range is [0,2^16-1])
    ~ uint32_t (range is [0,2^32-1])
    ~ uint64_t (range is [0,2^64-1], though the implementations of some of the color component operations
      involve conversion to long double, which isn't guaranteed to be an 80-bit floating point type, the
      63(+1) bit mantissa of which is necessary for some operations) -- this type is not really necessary though.
    ~ float (range is [0,1])
    ~ double (range is [0,1])
    ~ long double (range is [0,1]) -- this type is not really necessary though.
  * I'm considering abstracting these component types into something like the following:

      template <typename UnderlyingType, uint64_t MAX> class ColorComponent;

    which would represent a value in the range [0,MAX] using UnderlyingType to represent it.  This has
    two advantages:
    ~ It allows the color component operations to be more strongly typed, have safer operations/conversions,
      and carry semantic meaning around by virtue of the "ColorComponent" type name.
    ~ Abstracts away implementation details and leaves mostly just the color component operations in
      what the user must consider.

  * I'm considering also supplying a template parameter to RGB<T> and RGBA<T> to determine the order
    of the components within the memory layout, as this is a consideration in pixel-loading operations
    (e.g. in FreeImage and OpenGL texture-loading).
  * I want to add the following types to provide strongly-typed classes for different color spaces:
    ~ HSV<T> -- hue, saturation, value -- has the technical problem that hue is usually represented
      with a "degree" value in [0,360], but this goes against the color components' semantic of using
      an entire dynamic range (be it [0,255] for uint8_t, or [0,1] for float, etc).  Background context:
      the map from HSV -> RGB is a piecewise-linear map which splits the "hue" component up into 6
      subintervals of equal length.  While the normal max hue value 360 is divisible by 360, none of
      the color components' max values (e.g. uint8_t(255), uint16_t(65535), float(1), etc) are exactly
      divisible by 6.

      There are two solutions to this problem:
      1. This could be handled by mapping the dynamic range of the type onto [0,360], and not
         worrying about the fact that the 360 degrees don't split up perfectly in this scheme.
      2. Using a type that isn't a ColorComponent for the hue component.  The complications here
         would be the fact that uint8_t doesn't extend to 360, so it couldn't be used for the
         hue, so HSV<uint8_t> wouldn't really make sense.
    ~ HSVA<T> -- HSV<T> with an alpha channel.
    ~ Luminance<T> -- representing lightness values -- come up with a better name?
    ~ LuminanceAlpha<T> -- representing lightness and alpha values -- come up with a better name?
    ~ <some two-color-component type> -- FreeImage, for example, supports a "complex" pixel type.
    ~ CMYK<T> -- this is a common color space used in print (color pigment) -- this may not be called for.
    ~ CMYKA<T> -- CMYK<T> with an alpha channel.
  * There are a bunch of sub-byte-component color types in OpenGL (e.g. GL_UNSIGNED_BYTE_3_3_2,
    GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV,
    GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1,
    GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV,
    GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV) which would require nontrivial
    [un]packing to do operations on, and would natively correspond to a memory layout for these
    pixel formats.  Accounting for these may complicate the design too much to bother with them.

#### 2014.09.23 - Notes from GL Component meeting

- Get rid of GLController
- GLShaderLoader should not go in (because of the dependency on Resource/Singleton/ResourceManager)
- GLTexture2Loader should not go in (because of the dependency on Resource/Singleton/ResourceManager)
- Make GLMesh replace PrimitiveGeometry
- Abstract version of GLMaterial which has a run-time initialization specification of
  the uniform names/types, which does verification of types and so forth.
- Perhaps ModelView should just be an AffineTransform class which has no stack functionality,
  because SceneGraph has property stack functionality
- Perhaps Projection could be replaced with Camera and its use in SceneGraph.
- Abstracting the choice of Eigen out of the GL component
- GLShaderBindingScopeGuard is for thread safety but we may not need it
- Geometry shader is deemed not necessary right now
- Cube map -- worthwhile (code in Freeform) but low priority
- Pixel buffer objects may be a faster way to transfer pixels from GPU to CPU (low priority)
- GL reflection/traits -- useful but low priority
- Color space: RGB and RGBA -- and later once use cases are taken care of -- HSV, HSVA
- GLMesh
- Unit tests (this depends on SDLController or whatever is needed to create a GL context;
  could also make an interface for that purpose).

##### Design criteria

- GLCoreComponent should have example apps that do nontrivial rendering/demonstration.
  This provides an argument for including SceneGraph, Camera, Projection into GLCoreComponent

##### Other Components library considerations

- come up with a better resource system design
- Primitives looks like it could become its own package (with SceneGraph?)
- There may be a call for an agnostic abstraction for SDLController and SFMLController -- Pal in XRB
- Make RenderEngine component with Primitives -- and PrimitiveBase should probably be refactored out into Renderable.

##### Legal

- Need to start the open-sourcing process

##### Random notes

- Valve's GL debugging tool -- we probably want to benchmark/profile our library at least a little,
  so that we're not unwittingly releasing a slow piece of crap.

#### Prioritized list of considerations for GL component

##### GL component high priority

- Consistent GL resource (e.g. textures, buffers, etc) construction/[re]initialization/shutdown/destruction
  convention.  Some possible choices are:
  (1) Construction is resource acquisition, destruction is release (GLTexture2, GLShader does this)
  (2) Construction creates an "invalid/empty" resource, there is a separate Initialize/Create method,
      there is a separate Shutdown/Destroy method
      destruction releases the resource (GLBuffer does this).
  (3) Construct with acquired resource (as in (1)) or construct as "invalid/empty",
      there is a [Re]Initialize method to [re]acquire a resource
      there is a Shutdown method
      destruction releases the resource.
  Number (3) is probably the most flexible, because it doesn't require destroying and reconstructing
  to change what resource something points to.  However, use of std::shared_ptr may make this unnecessary.
  Then again, we probably don't want to make that architectural choice for people, and want our
  classes to be usable in many different paradigms.
- DONE: Make the directory structure of the components repo into the following, where an X indicates a change.

  * components/                             -- root directory of repo -- this exists already
  * components/library/                   X -- new root directory of the Components library
  * components/library/CMakeLists.txt     X -- this is where you'd point cmake to build the Components library
  * components/library/source/            X -- this is where the existing components/source/ directory would go
  * components/example_apps/                -- directory containing all example apps -- this exists already.

  I think having a "library" subdir of components is clearer, because then it reads
  "components library", which is what it is.  Then the source code for the library
  is in the "source" subdirectory of "library", which is also what that is.
- DONE: Namespace Leap::GL::
- DONE: Directory structure to reflect the namespace:
  e.g. components/library/source/Leap/GL/Abc.h
- DONE: Get rid of GLController, as it does almost nothing, and its original intended design (to
  track OpenGL server state and prevent redundant server state changes) is contrary to one
  of the main design principles of the Components lib (drop-in capability).
- DONE: Create GLMesh and factor out of PrimitiveGeometry.
- DONE: Abstracted version of GLMaterial that is similar to the design of GLVertexBuffer, but does
  initialization of the material at runtime (parallel to the concept of shaders being compiled
  and linked at runtime).
- DONE: Ensure that all types of uniforms can be set via GLShader (in particular, arrays and structures of uniforms).
- Add SceneGraph and Camera.  Keep Projection [matrix], get rid of ModelView (because its stack
  is replaced by SceneGraph) and perhaps use an AffineTransform<DIM> class instead.
- Abstracting the choice of a particular linear algebra library (Eigen in our case) out.
  This will require some prototyping and code review.
- DONE: Determine if exception safety is a good enough reason to include GLShaderBindingScopeGuard,
  otherwise get rid of it.  It has been decided that exception safety, along with a uniformized
  resource binding/unbinding convention is a good enough reason to have this.
- DONE: Color -- RGB<T> and RGBA<T> (but do HSV<T> and HSVA<T> later)
- All color unit tests should pass (some are currently disabled).
- Integrate RGB and RGBA into rest of code, replacing "class Color"
- Unit tests (this depends on SDLController or whatever is needed to create a GL context;
  could also make an interface for that purpose -- perhaps that "make me a GL context" interface
  would be useful in the GL core component?).
- Full, Doxygen-based documentation.

##### GL component low priority

- Geometry shaders maybe some day.
- Cube map (a lot of code is in Freeform)
- Pixel buffer objects (could be a way to do faster pixel transfers from GPU to CPU)
- GLTraits / reflection
- Color -- HSV<T>, HSVA<T>, sRGB<T>, sRGBA<T> (sRGB is a nonlinear analog to RGB)
- Uniform buffer objects (this appears to be for OpenGL 4+ (?))

#### GL component closure notes

The GL component is designed to provide abstractions of concepts in the OpenGL API.
The purpose of this analysis is to determine a closed set of features/API for each
abstraction, based on the OpenGL API.  E.g. glTexImage2D is affected by state that
is controlled by glPixelStore* and glTexParameter*, and therefore the GLTexture2
class must provide an API for using those capabilities in the abstraction.

FrameBufferObject (rename to Leap::GL::Framebuffer) 
- Related concepts
  * pixel buffer object (interesting exerpt from the OpenGL wiki:
    PBOs have nothing to do with Framebuffer Objects. Note the capitalization;
    "framebuffer" is one word. FBOs are not buffer objects; PBOs are. FBOs are
    about rendering to off-screen images; PBOs are about pixel transfers to/from
    the user from/to images in OpenGL. They are not alike.)
- List of relevant GL calls in existing code
  * glBindFramebuffer
  * glBlitFramebuffer
  * glGenFramebuffers (Not in OpenGL 2.1, but is in OpenGL 3.3)
  * glDeleteFramebuffers
  * glCheckFramebufferStatus
  * glFramebufferTexture2DEXT
  * glFramebufferRenderbufferEXT
- List of other relevant GL calls
  * glFramebufferRenderbuffer
  * glFramebufferTexture (OpenGL 3.2)
  * glFramebufferTexture1D
  * glFramebufferTexture2D
  * glFramebufferTexture3D
  * glBlitFramebuffer
  * glReadBuffer
  * glDrawBuffer / glDrawBuffers
  * glReadPixels (can optionally specify offset into pixel buffer object)
  * TODO: examine API docs for closure
- Related calls
  * glClear
  * glClearColor
  * glClearDepth
  * glClearStencil
  * glClearBuffer*
  * glColorMask
  * glDepthMask
  * glStencilMask
  * glClampColor
  * glTexImage2DMultisample (OpenGL 3.2)
- Associated glGet calls
  * glGetIntegerv(GL_MAX_SAMPLES_EXT, ...)
  * glGetFramebufferAttachmentParameteriv
  * TODO: examine API docs for closure

GLBuffer (rename to Leap::GL::Buffer)
- List of relevant GL calls
  * glGenBuffers
  * glBindBuffer
  * glBufferData
  * glBufferSubData
  * glMapBuffer
  * glUnmapBuffer
  * glDeleteBuffers
  * TODO: examine API docs for closure
- Associated glGet calls
  * TODO: examine API docs for closure

GLMaterial (rename to Leap::GL::Material)
- This is an abstraction completely on top of GLShader, so it doesn't call OpenGL directly.
- TODO: examine API docs for closure

GLShaderMatrices (rename to Leap::GL::ShaderMatrices or perhaps to X, where X is to vertex shader
where GLMaterial is to fragment shader).
- This is an abstraction completely on top of GLShader, so it doesn't call OpenGL directly.
- TODO: examine API docs for closure

GLShader (rename to Leap::GL::Shader)
- List of relevant GL calls
  * glUseProgram
  * glUniform*
  * glMatrixUniform*
  * glCreateProgram
  * glAttachShader
  * glLinkProgram
  * glDeleteProgram
  * glDeleteShader
  * glCreateShader
  * glShaderSource
  * glCompileShader
  * TODO: examine API docs for closure
- Associated glGet calls
  * glGetIntegerv(GL_CURRENT_PROGRAM, ...)
  * glGetProgramiv(<program-handle>, GL_ACTIVE_UNIFORMS, ...)
  * glGetProgramiv(<program-handle>, GL_ACTIVE_UNIFORM_MAX_LENGTH, ...)
  * glGetActiveUniform
  * glGetUniformLocation
  * glGetProgramiv(<program-handle>, GL_ACTIVE_ATTRIBUTES, ...)
  * glGetProgramiv(<program-handle>, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ...)
  * glGetActiveAttrib
  * glGetAttribLocation
  * glGetShaderiv(<shader-handle>, GL_COMPILE_STATUS, ...)
  * glGetShaderiv(<shader-handle>, GL_INFO_LOG_LENGTH, ...)
  * glGetShaderInfoLog
  * TODO: examine API docs for closure

GLTexture2 (rename to Leap::GL::Texture2)
- List of relevant GL calls
  * glBindTexture
  * glPixelStore*
  * glGenTextures
  * glTexParameter*
  * glTexImage2D
  * glDeleteTextures
  * glTexSubImage2D
  * glGetTexImage (doesn't really count as a glGet call, since it's really heavyweight)
  * TODO: examine API docs for closure
- Associated glGet calls
  * glGetIntegerv with pixel store parameter enums
  * glGetTexLevelParameteriv (with e.g. GL_TEXTURE_INTERNAL_FORMAT)
  * TODO: examine API docs for closure

GLVertexAttribute (rename to Leap::GL::VertexAttribute)
- List of relevant GL calls
  * glEnableVertexAttribArray
  * glVertexAttribPointer
  * glDisableVertexAttribArray
  * TODO: examine API docs for closure
- Associated glGet calls
  * TODO: examine API docs for closure

GLVertexBuffer (rename to Leap::GL::VertexBuffer)
- This is an abstraction completely on top of GLBuffer and GLVertexAttribute, so it doesn't call OpenGL directly.
- TODO: examine API docs for closure

RenderBuffer (rename to Leap::GL::RenderBuffer)
- List of relevant GL calls
  * glBindRenderbuffer
  * glGenRenderbuffers
  * glRenderbufferStorage
  * glRenderbufferStorageMultisample
  * TODO: examine API docs for closure
- Associated glGet calls
  * glGetIntegerv(GL_MAX_SAMPLES_EXT, ...)
  * TODO: examine API docs for closure

#### GL component resource conventions notes

The purpose of this section is to come up with a uniform set of conventions for how
OpenGL-based resources (e.g. textures, vertex buffers, etc) are created/destroyed/
bound/unbound/reinitialized/shutdown such that it makes the fewest assumptions about
how the objects are used (e.g. assuming that they are always constructed via new).

The resource-based OpenGL concepts that we will provide abstractions for in the GL
component are the following.
    
- Texture2
- Shader
- Buffer
- VertexBuffer
- Mesh
- Material
- FrameBuffer
- RenderBuffer

Some possibilities for resource conventions are the following.

1.  Construction is resource acquisition, destruction is release (GLTexture2, GLShader does this),
    and if a failure occurs during resource acquisition (which is the same as construction), an
    exception is thrown.
2.  Construction creates an "invalid/empty" resource, there is a separate Initialize/Create method,
    there is a separate Release/Destroy method
    destruction releases the resource (GLBuffer does this).
3.  Construct with acquired resource (as in (1)) or construct as "invalid/empty",
    there is a [Re]Initialize method to [re]acquire a resource
    there is a Release method
    destruction releases the resource.

Number (1) is the most pure in a type-theoretic sense, because it makes impossible construction of
an invalid/empty resource.  However not being able to release/reinitialize a resource from an
already-allocated object prevents certain use cases.

Number (2) requires the most code to use, as construction does no initialization (resource acquisition).
Having initialization be separate from construction can be useful for when acquiring the resource is
impossible during the time of construction.

Number (3) is probably the most flexible, because it doesn't require destroying and reconstructing
to change what resource something points to.  However, use of std::shared_ptr may make this unnecessary.
Then again, we probably don't want to make that architectural choice for people, and want our
classes to be usable in many different paradigms.  It should be possible to use the classes as
local variables, as members, as objects allocated on the heap, via std::shared_ptr, etc.

There should (could?) be an Update method as well, which re-initializes the resource, and probably
corresponds to a relatively lightweight operation (compared to releasing and re-initializing).  Perhaps
this should be resource-specific, so no requirement is made on it.

The Release method should take no arguments, so that it can be called without specific knowledge of
what type the resource is.

Each OpenGL resource has associated "bind"/"unbind" operations which affect the state of the OpenGL server
and consequent drawing operations.

A "scope guard" is a design pattern which uses RAII to guarantee that a bound resource is unbound no later
than the end of the scope in which it's created.  This should be the standard way to bind/unbind resources.
In fact, the resources' bind/unbind operations could be designed to require the use of a scope guard (use
of a scope guard wouldn't be absolutely required -- there should be some way to do "manual" binding/
unbinding).

Finally, in order to maximize flexibility, each resource class should ideally be populatable from existing,
created-using-raw-OpenGL-calls resources.  For example, a 2D texture created in some other library that
the user wants to control via GLTexture2 -- there should be a means to initialize a GLTexture2 using the
existing texture handle and other parameters.

#### Evaluation of existing C++ OpenGL wrapper libraries

- OOGL    : https://github.com/Overv/OOGL
  * Provides a small and lightweight set of C++ classes which wrap some common OpenGL
    concepts, similar to the intended design of Leap's GL component, though not nearly
    as extensive as ours.
  * Is reasonably strongly typed, using different C++ enums for different functions
    instead of GLenums.
  * Appears to not be actively developed -- the last commit to the git repo was about a
    year before the writing of this note, and the last substantial work on it was about
    two years before the writing of this note.
- OGLplus : http://oglplus.org/
  * Very sophisticated C++ library wrapping OpenGL 3+.
  * Template-heavy with a very involved design which makes its source code rather difficult
    to read, though it is clearly very well-thought out.
  * It's not clear if this is actually used in any projects, so its completeness is unknown.
  * Is currently actively developed.
- GLT     : http://www.nigels.com/glt/
  * Provides a set of C++ classes to wrap much of the functionality provided by OpenGL
  * Seems to be missing some key concepts, notably shaders.
  * Is straightforward code, so it's relatively easy to read.
  * Also appears to not be actively developed (last update was in 2012), and may have
    existed since before 2003, which would explain why some concepts are missing.

#### GLMesh design notes

- Draw mode (e.g. GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_LINES, etc.).
  GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
  GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY and
  GL_TRIANGLES_ADJACENCY.
- Two modes for vertex specification (this would be a template parameter because it determines
  the [non-]existence of intermediate vertex data):
  * Indexed (uses glDrawElements) -- intermediate vertex data necessary, common vertices will
    be collapsed when uploaded to vertex buffer.
  * Direct (uses glDrawArrays) -- no intermediate vertex data necessary.
- Should be able to specify what vertex attributes are present
  * Position : required
  * Normal   : optional (but this class should be aware of it)
  * Others   : optional user-specified attributes, e.g. texture coordinates, color, etc.
- The position and normal attributes have some dimension configurability.
  * 3d position, 3d normal (normals defined to be normal to the surface)
  * 2d position, 3d normal (normals defined to be the unit Z vector, normal to the XY plane)
  * 2d position, 2d normal (the mesh would (could) represent the 1d boundary of a 2d region,
    and the normals would be normal to that 1d boundary curve).  While this is mathematically
    nice, it's probably the least useful in terms of graphics.
- There should be methods for adding surface primitives
  * Vertex
  * Line
  * Line strip vertex
  * Triangle
  * Triangle strip vertex
  * Triangle fan vertex
  * Quad (just adds 2 triangles) -- this may be difficult to design to play nicely with the
    different triangles
  * TODO: Figure out if there should be methods for GL_TRIANGLES_ADJACENCY and other adjacency
    modes.
- It might also be useful to be able to add vertices and indices manually, with no intermediate
  vertex storage, e.g. loading a mesh from a file, where the vertex indices are precomputed.
- Perhaps there should be a method which produces the induced wireframe for a mesh (or can
  that be done with a shader?).

Other random notes
- Halfedge structure for storing meshes and determining adjacency.  This gives a consistent
  and efficient way to represent oriented surfaces, and probably naturally generalizes to
  arbitrary complexes.

#### GLMaterial abstraction design notes

Generally what is needed is a strongly-typed C++ frontend for setting uniforms in GLSL
shaders.  GLShader is aware of what uniforms and attributes are present in the program,
and what type each one is.

Perhaps this concept should be called GLShaderFrontend.  GLShaderFrontend will expect
particularly named/typed uniforms to be present in the currently bound shader, and has
methods for setting those uniform values in a strongly typed way.  There are two methods
for delivering uniform values:

- Immediate mode -- the specified uniform values are set in the shader directly, without
  storing the values.
- Cached mode -- the GLShaderFrontend object can store values persistently, and those
  values will be set in the shader in the "upload uniforms" operation.

Both methods of delivering uniforms will use the same "packet type", so that the interface
for specifying uniform values is contained within a single place, and not duplicated
in GLShaderFrontend.

#### Design notes for hooked GLController (different than existing/deprecated GLController)

The goal is to provide a minimal but clear C++ interface to the GL state, caching 
certain state variables with the purpose of minimizing number of GL calls.  A limited 
set of function calls control the state, and some functions' effects depend on particular 
state variables (e.g. glUniform*, controlling values for the currently loaded shader program).

NOTE: One of the goals of the code created by this Components team is to create code that
is highly modularized, having minimal dependency.  If a full state-tracking GL class were made,
it would essentially be incompatible with anything that does "raw" GL calls, since this would
modify the GL state without the knowledge of the C++ GL state object.

If it were possible to "reroute" all GL calls through "our" functions (the corresponding methods
called on the GL state singleton object), then no code would need to be changed for such a
facility to be used, including that of library code, which should be considered a design
criteria.  The advantage is then we would in principle be aware of and control all GL calls.
The disadvantage is that what appear to be "plain old C function calls" then gain this new,
mysterious, and possibly unexpected behavior.

Some relevant info:

Wrapping symbols via a linker step:
http://stackoverflow.com/questions/13961774/gnu-gcc-ld-wrapping-a-call-to-symbol-with-caller-and-callee-defined-in-the-sam

This technique could be used in other ways, such as replacing malloc/calloc/free.




