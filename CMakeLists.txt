include(TargetImportedLibraries) # for Walter's target_package command

# A "component" can be thought of as a "sub-library" (in the sense that it is a small library and
# is subordinate to the whole library).  A component satisfies two requirements:
# - Has a well-defined purpose, scope, and feature set.
# - Has well-defined dependencies, which are explicitly declared and are minimal.
#
# This is the list of defined components.  The component name should be identical to
# the subdirectory which contains all its source files.  A component name should be
# a C identifier that is WordCapitalized.  Each component should have
# the following macros defined.
#   Foo_SOURCES                         The list of source files for component "Foo"
#   Foo_EXPLICIT_COMPONENT_DEPENDENCIES The list of components which component "Foo" explicitly depends on; not,
#                                       for example, components which "Foo" depends on through other components.
#                                       It's ok if there are redundancies here.
#   Foo_EXPLICIT_LIBRARY_DEPENDENCIES   The list of library-version pairs which component "Foo" explicity depends
#                                       on; not, for example, libraries which "Foo" depends on through other
#                                       components or libraries.  It's ok if there are redundancies here.
# The total component dependencies of a component can be determined recursively using these
# definitions.  Similarly, the total library dependencies of a component can be determined.

# TODO: make a function that defines a whole component

# COMPONENT_LIST is a space-delimited string enumerating all the defined components.  Each component
# should append to it in its definition section (see below).  It is initialized here to empty string
# so that its existence and purpose is clear.
set(COMPONENT_LIST "")

###################################################################################################
# The Application component provides a simple framework for an SDL-based application.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} Application)
set(Application_SOURCES
    Application.cpp
    Application.h
    EventHandler.h
    Renderable.h
    RenderableEventHandler.h
)
set(Application_EXPLICIT_COMPONENT_DEPENDENCIES
    # none
)
set(Application_EXPLICIT_LIBRARY_DEPENDENCIES
    SDL 2
)

###################################################################################################
# The GLCompatibility component provides means for including OpenGL (and related) headers in
# a platform-agnostic way.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} GLCompatibility)
set(GLCompatibility_SOURCES
    gl_glext_glu.h
)
set(GLCompatibility_EXPLICIT_COMPONENT_DEPENDENCIES
    # none
)
set(GLCompatibility_EXPLICIT_LIBRARY_DEPENDENCIES # this will probably depend on the platform
    gl
    glext
    glu
)

###################################################################################################
# The GLController component provides a single point of control for usage of OpenGL.
###################################################################################################

# This component is currently not fully designed or implemented.  Perhaps some "in-progress"
# directory for components is needed.
# set(COMPONENT_LIST ${COMPONENT_LIST} GLController) # Commenting out this line disables the component
set(GLController_SOURCES
    GLController.cpp
    GLController.h
)
set(_EXPLICIT_COMPONENT_DEPENDENCIES
    GLCompatibility
)
set(_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The GLShader component provides a C++ class which handles shaders -- compiling, binding, 
# unbinding, setting uniforms.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} GLShader)
set(GLShader_SOURCES
    GLShader.cpp
    GLShader.h
)
set(GLShader_EXPLICIT_COMPONENT_DEPENDENCIES
    GLCompatibility
)
set(GLShader_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The GLShaderLoader component provides ResourceManager-managed loading of GLShader via shader
# source files.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} GLShaderLoader)
set(GLShaderLoader_SOURCES
    GLShaderLoader.h
)
set(GLShaderLoader_EXPLICIT_COMPONENT_DEPENDENCIES
    GLShader
    Resource
    TextFileLoader
)
set(GLShaderLoader_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The GLTexture2 component provides a C++ class for managing 2D textures in OpenGL.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} GLTexture2)
set(GLTexture2_SOURCES
    GLTexture2.cpp
    GLTexture2.h
)
set(GLTexture2_EXPLICIT_COMPONENT_DEPENDENCIES
    GLCompatibility
)
set(GLTexture2_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The GLTexture2FreeImage component provides functionality for creating a GLTexture2 from
# an image file using the FreeImage library.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} GLTexture2FreeImage)
set(GLTexture2FreeImage_SOURCES
    GLTexture2FreeImage.cpp
    GLTexture2FreeImage.h
)
set(GLTexture2FreeImage_EXPLICIT_COMPONENT_DEPENDENCIES
    GLTexture2
)
set(GLTexture2FreeImage_EXPLICIT_LIBRARY_DEPENDENCIES
    FreeImage 3
)

###################################################################################################
# The GLTexture2Loader component provides ResourceManager-managed loading of GLTexture2 objects
# using the FreeImage library.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} GLTexture2Loader)
set(GLTexture2Loader_SOURCES
    GLTexture2Loader.cpp
    GLTexture2Loader.h
)
set(GLTexture2Loader_EXPLICIT_COMPONENT_DEPENDENCIES
    ResourceManager
    GLTexture2
    GLTexture2FreeImage
    SDLController
)
set(GLTexture2Loader_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The Resource component provides convenient loading of ResourceManager-managed typed resources
# via a typed ResourceManager singleton.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} Resource)
set(Resource_SOURCES
    Resource.h
)
set(Resource_EXPLICIT_COMPONENT_DEPENDENCIES
    ResourceManager
    Singleton
)
set(Resource_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The ResourceManager component provides a means for loading and caching resources in a
# non-redundant way.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} ResourceManager)
set(ResourceManager_SOURCES
    ResourceManager.h
)
set(ResourceManager_EXPLICIT_COMPONENT_DEPENDENCIES
    # none
)
set(ResourceManager_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The SDLController component provides a single point of control for initializing, shutting down,
# and general usage of SDL.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} SDLController)
set(SDLController_SOURCES
    SDLController.cpp
    SDLController.h
)
set(SDLController_EXPLICIT_COMPONENT_DEPENDENCIES
    # none
)
set(SDLController_EXPLICIT_LIBRARY_DEPENDENCIES
    SDL 2
)

###################################################################################################
# The TextAndBinaryFile component provides simple C++ classes for loading and storing text and
# binary files (primarily intended to be used with Resource).
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} TextAndBinaryFile)
set(TextAndBinaryFile_SOURCES
    TextFile.h
)
set(TextAndBinaryFile_EXPLICIT_COMPONENT_DEPENDENCIES
    # none
)
set(TextAndBinaryFile_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# The TextAndBinaryFileLoader component provides ResourceManager-managed loading of text and
# binary files.
###################################################################################################

set(COMPONENT_LIST ${COMPONENT_LIST} TextAndBinaryFileLoader)
set(TextAndBinaryFileLoader_SOURCES
    TextFileLoader.h
)
set(TextAndBinaryFileLoader_EXPLICIT_COMPONENT_DEPENDENCIES
    ResourceManager
    SDLController
    TextAndBinaryFile
)
set(TextAndBinaryFileLoader_EXPLICIT_LIBRARY_DEPENDENCIES
    # none
)

###################################################################################################
# Here are the build rules for the overall library
###################################################################################################

# NOTE: these are not being compiled in because they currently have too many unmet dependencies
# and require a nontrivial amount of integration work.
# # Please keep these alphabetized.
# set(SOURCE_THAT_MUST_STILL_BE_INTEGRATED
#     # Color.cpp # has other dependencies
#     # Color.h
#     DataTypes.h
#     # FrameBufferObject.cpp # depends on SimGL.h which has other dependencies
#     # FrameBufferObject.h
#     # GLBuffer.cpp # has other dependencies
#     # GLBuffer.h
#     OculusVR.cpp
#     OculusVR.h
#     Primitive.cpp
#     Primitive.h
#     RenderBuffer.cpp
#     RenderBuffer.h
#     # SimGL.cpp # has other dependencies
#     # SimGL.h
#     Texture.cpp 
#     Texture.h
#     UtilGL.cpp
#     UtilGL.h
#     Utilities.h
# )

# TODO: write functinos for the following:
# - Determining all component dependencies of a component (recursively).
# - Determining all library dependencies of a component (recursively).
# - Determining the include directories for a component.
# - Determining the library link options for a component.
# - Calling the appropriate function for defining a library for the component.

set(Components_SOURCES "")
set(Components_INCLUDE_DIRECTORIES "")
foreach(COMPONENT ${COMPONENT_LIST})
    # message("adding sources for component ${COMPONENT}")
    foreach(SOURCE_FILE ${${COMPONENT}_SOURCES})
        set(SOURCE_PATH ${COMPONENT}/${SOURCE_FILE})
        # message("    adding ${SOURCE_PATH}")        
        set(Components_SOURCES ${Components_SOURCES} ${SOURCE_PATH})
    endforeach()
    # message("adding include directory for component ${COMPONENT}")
    set(Components_INCLUDE_DIRECTORIES ${Components_INCLUDE_DIRECTORIES} ${COMPONENT})
endforeach()

# # TEMP: for now, just add all the yet-to-be-integrated source
# set(Components_SOURCES ${Components_SOURCES} ${SOURCE_THAT_MUST_STILL_BE_INTEGRATED})

message("Components_INCLUDE_DIRECTORIES = ${Components_INCLUDE_DIRECTORIES}")
message("Components_SOURCES = ${Components_SOURCES}")

# TEMP: for now, just include all source and all include directories
include_directories(${Components_INCLUDE_DIRECTORIES})
add_library(Components ${Components_SOURCES})

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall -Werror")
endif()

# TEMP: for now, just link all library dependencies.
target_package(Components Eigen 3.1.2 REQUIRED)
target_package(Components OpenGL REQUIRED)
target_package(Components FreeImage REQUIRED)
target_package(Components SDL 2 REQUIRED)




