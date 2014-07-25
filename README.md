#### Components Source Code Organization

This components library is a set of components; "component" is defined as a set of source files that:
- Has a well-defined purpose, scope, and feature set.
- Has well-defined dependencies, which are explicitly declared and are minimal.

To that end, each component's source code goes in its own separate directory so that the boundaries
between components are utterly clear; they can be encoded in not only the directory structure but
also the CMakeLists.txt include directories.  Each component can even have its own README.md file
(or even Doxygen doc set).

TODO: There should be some cmake-based command where a set of components is specified, and then 
those components are compiled into a single library, and the set of include directories for those
components is returned to the caller.  This would allow a minimal, tailor-made "components"
library to be generated (e.g. a library that only includes ResourceManager, Resource, Singleton,
and SDLController).  This feature depends on and will exercise the above strict organization of
components.

