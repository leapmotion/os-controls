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


