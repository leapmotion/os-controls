cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

###################################################################################################
# High-level project definitions
###################################################################################################

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.8) # all glu functions are depricated in 10.9
project(Components)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

# TODO: set a default value for CMAKE_INSTALL_PREFIX to something like
# Components-versionnumber
# TODO: also look into cpack (for making Mac frameworks, Windows NSIS stuff, deb files, etc.)

###################################################################################################

# Module path and module includes
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})                      # For DefineComponents.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake-modules")   # For most other cmake modules

###################################################################################################
# We use an EXTERNAL_LIBRARY_DIR variable as a hint to where all the libraries can be found.
# This is an optional means to not have to specify each library's root dir directly.
###################################################################################################

find_path(EXTERNAL_LIBRARY_DIR "glew-1.9.0" HINTS /opt/local/Libraries PATHS $ENV{PATH} $ENV{EXTERNAL_LIBRARY_DIR} NO_DEFAULT_PATH)

# TODO: Make EXTERNAL_LIBRARY_DIR detection optional, since users may not have their libraries
# installed the same way we (Leap) do.
if(EXTERNAL_LIBRARY_DIR STREQUAL "EXTERNAL_LIBRARY_DIR-NOTFOUND")
    message(STATUS "External Library Directory not found, please specify a folder to look for external libraries")
    return()
endif()

# CMAKE_PREFIX_PATH is the path used for searching by FIND_XXX(), with appropriate suffixes added.
# EXTERNAL_LIBRARY_DIR is a hint for all the find_library calls.
list(INSERT CMAKE_PREFIX_PATH 0 ${EXTERNAL_LIBRARY_DIR})

###################################################################################################
# Component definitions
###################################################################################################

# This section defines what a component is and defines helper macros.

###################################################################################################
# This call initializes the DEFINED_COMPONENTS var to empty -- defined components will be added to it.
# NOTE: Please keep the component definitions in alphabetical order by component name.
###################################################################################################

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
#   Foo_INSTALL_FILES                   The list of files to copy into a "release" archive.
#   Foo_EXPLICIT_COMPONENT_DEPENDENCIES The list of components which component "Foo" explicitly depends on; not,
#                                       for example, components which "Foo" depends on through other components.
#                                       It's ok if there are redundancies here.
#   Foo_EXPLICIT_LIBRARY_DEPENDENCIES   The list of library-version pairs which component "Foo" explicity depends
#                                       on; not, for example, libraries which "Foo" depends on through other
#                                       components or libraries.  It's ok if there are redundancies here.
# The total component dependencies of a component can be determined recursively using these
# definitions.  Similarly, the total library dependencies of a component can be determined.

include(DefineComponents)

begin_component_definitions()

###################################################################################################
# Component lib'ary definitions
###################################################################################################

define_component_as_library(
    C++11
    COMPILE_OPTIONS
        -std=c++11
    BRIEF_DOC_STRINGS
        "The C++11 component is a phony target which just provides compiler options for enabling C++11."
)

define_component_as_library(
    Application
    HEADERS
        Application.h
        EventHandler.h
        Renderable.h
        RenderableEventHandler.h
    SOURCES
        Application.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRINGS
        "The Application component provides a simple framework for an SDL-based application."
)

define_component_as_library(
    EigenTypes
    HEADERS
        EigenTypes.h
    EXPLICIT_LIBRARY_DEPENDENCIES
        "Eigen 3.1.2"
    BRIEF_DOC_STRINGS
        "The EigenTypes component provides convenience typedefs for working with Eigen matrices."
)

define_component_as_library(
    ResourceManager
    HEADERS
        ResourceManager.h
    BRIEF_DOC_STRINGS
        "The ResourceManager component provides a means for loading and caching resources in a\n"
        "non-redundant way."
)

# NOTE: MakeTransparent_Apple.mm is an Objective-C++ source file which should only be compiled
# on Apple platforms.  It should be ignored on other platforms.
define_component_as_library(
    SDLController
    HEADERS
        SDLController.h
    SOURCES
        MakeTransparent_Apple.mm
        SDLController.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRINGS
        "The SDLController component provides a single point of control for initializing, shutting down,\n"
        "and general usage of SDL."
)

define_component_as_library(
    FbxSceneLoader
    HEADERS
        FbxSceneLoader.h
    SOURCES
        FbxSceneLoader.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        ResourceManager
        SDLController
    EXPLICIT_LIBRARY_DEPENDENCIES
        "FbxSdk" # TODO: specify version (IMPORTANT: inside the quotes, e.g. "FbxSdk 2015.1")
    BRIEF_DOC_STRINGS
        "The FbxSceneLoader component provides ResourceManager-managed loading of FbxScene objects\n"
        "using the FBX SDK library."
)

define_component_as_library(
    GLCompatibility
    HEADERS
        gl_glext_glu.h
    EXPLICIT_LIBRARY_DEPENDENCIES
        "OpenGL 2.1"
        "Glew 1.9.0"
    BRIEF_DOC_STRINGS
        "The GLCompatibility component provides means for including OpenGL (and related) headers in\n"
        "a platform-agnostic way."
)

define_component_as_library(
    GLTexture2
    HEADERS
        GLTexture2.h
    SOURCES
        GLTexture2.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        C++11
        GLCompatibility
    BRIEF_DOC_STRINGS
        "The GLTexture2 component provides a C++ class for managing 2D textures in OpenGL."
)

define_component_as_library(
    FrameBufferObject
    HEADERS
        FrameBufferObject.h
        RenderBuffer.h
    SOURCES
        FrameBufferObject.cpp
        RenderBuffer.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        GLTexture2
        GLCompatibility
    BRIEF_DOC_STRINGS
        "The FrameBufferObject component provides a way to render to a render buffer or texture."
)

define_component_as_library(
    GLBuffer
    HEADERS
        GLBuffer.h
    SOURCES
        GLBuffer.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        GLCompatibility
    BRIEF_DOC_STRINGS
        "The GLBuffer component provides a fast GPU buffer object for geometry."
)

define_component_as_library(
    GLController
    HEADERS
        GLController.h
    SOURCES
        GLController.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        GLCompatibility
    BRIEF_DOC_STRINGS
        "The GLController component provides a single point of control for usage of OpenGL."
)

define_component_as_library(
    GLShader
    HEADERS
        GLShader.h
    SOURCES
        GLShader.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        C++11
        GLCompatibility
    BRIEF_DOC_STRINGS
        "The GLShader component provides a C++ class which handles shaders -- compiling, binding,\n"
        "unbinding, setting uniforms."
)

define_component_as_library(
    Singleton
    HEADERS
        Singleton.h
    BRIEF_DOC_STRINGS
        "The Singleton component provides a simple mechanism for managing singleton C++ objects, via\n"
        "class inheritance or not."
)

define_component_as_library(
    Resource
    HEADERS
        Resource.h
    EXPLICIT_COMPONENT_DEPENDENCIES
        ResourceManager
        Singleton
    BRIEF_DOC_STRINGS
        "The Resource component provides convenient loading of ResourceManager-managed typed resources\n"
        "via a typed ResourceManager singleton."
)

define_component_as_library(
    TextAndBinaryFile
    HEADERS
        TextFile.h
    BRIEF_DOC_STRINGS
        "The TextAndBinaryFile component provides simple C++ classes for loading and storing text and\n"
        "binary files (primarily intended to be used with Resource)."
)

define_component_as_library(
    TextAndBinaryFileLoader
    HEADERS
        TextFileLoader.h
    EXPLICIT_COMPONENT_DEPENDENCIES
        ResourceManager
        SDLController
        TextAndBinaryFile
    BRIEF_DOC_STRINGS
        "The TextAndBinaryFileLoader component provides ResourceManager-managed loading of text and\n"
        "binary files."
)

define_component_as_library(
    GLShaderLoader
    HEADERS
        GLShaderLoader.h
    EXPLICIT_COMPONENT_DEPENDENCIES
        GLShader
        Resource
        TextAndBinaryFileLoader
    BRIEF_DOC_STRINGS
        "The GLShaderLoader component provides ResourceManager-managed loading of GLShader via shader\n"
        "source files."
)

define_component_as_library(
    GLTexture2FreeImage
    HEADERS
        GLTexture2FreeImage.h
    SOURCES
        GLTexture2FreeImage.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        GLTexture2
    EXPLICIT_LIBRARY_DEPENDENCIES
        "FreeImage 3"
    BRIEF_DOC_STRINGS
        "The GLTexture2FreeImage component provides functionality for creating a GLTexture2 from\n"
        "an image file using the FreeImage library."
)

define_component_as_library(
    GLTexture2Loader
    HEADERS
        GLTexture2Loader.h
    SOURCES
        GLTexture2Loader.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        GLTexture2FreeImage
        ResourceManager
        SDLController
    BRIEF_DOC_STRINGS
        "The GLTexture2Loader component provides ResourceManager-managed loading of GLTexture2 objects\n"
        "using the FreeImage library."
)

define_component_as_library(
    Primitives
    HEADERS
        Primitive.h
        Rectangle.h
    SOURCES
        Primitive.cpp
        Rectangle.cpp
    EXPLICIT_COMPONENT_DEPENDENCIES
        C++11
        EigenTypes
    BRIEF_DOC_STRINGS
        "The Primitives component provides some simple shapes in a transform hierarchy."
)

###################################################################################################
# Loader path stuff (necessary for correct loading of dylibs when the dylib files are moved).
###################################################################################################

# TODO: make these into target properties.  BUILD_WITH_INSTALL_RPATH is a target property.
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# This specifies the directory prefix for the install name (see output of `otool -L libComponents.dylib`).
# Shared libraries should use "@loader_path", and can be installed side by side with the application
# binary.  See the Stub example app for usage.
set(CMAKE_INSTALL_NAME_DIR "@loader_path")

###################################################################################################
# Global compiler options (and related) go here.  NOTE: ANY COMPONENT-SPECIFIC COMPILER OPTIONS
# SHOULD BE MADE IN THE CORRESPONDING COMPONENT DEFINITION!
###################################################################################################

# TODO: look into using target_compile_definitions and target_compile_options,
# though these seem sufficiently global to warrant the extreme measure of
# setting CMAKE_CXX_FLAGS.

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

    # This ugly block of code is to conditionally add the -Wno-deprecated-register option
    # for versions of Clang newer than 4.2.
    # TODO: there's probably some "real" way to get the compiler version,
    # or really just handle the -Wno-deprecated-register option more gracefully.
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
    string(REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
    # message("CLANG_VERSION_STRING = ${CLANG_VERSION_STRING}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall -Werror")
    if (CLANG_VERSION_STRING VERSION_GREATER 4.2)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") # This is GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX") # /WX is to treat warnings as errors.
endif()

# Special rules to handle use of the libc++ library on Mac with Clang.
# TODO: Are there analogous rules for Mac with GCC?
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # GCC ("GNU") probably would require a different option
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

###################################################################################################
# Definition of the actual Components library -- this is the target that will be used by external
# projects that link this library.
###################################################################################################

# TODO: We could build both static and shared libraries.  There will be a name collision in
# the library target, which we can resolve using
# http://datainfer.wordpress.com/2013/10/24/make-both-static-and-shared-libraries-in-one-build-with-cmake/
# However I haven't implemented this directly because there is a different name collision
# problem in multiple calls to target_package to specify dependencies (e.g. on Eigen).

# TODO: could/should this be a target property?
option(BUILD_SHARED_LIBS "Build Components as a shared library (instead of static)" OFF)

add_library(Components empty.cpp)
foreach(_component ${DEFINED_COMPONENTS})
    target_link_libraries(Components PUBLIC ${_component})
endforeach()

###################################################################################################
# Export rule for creating the ComponentsConfig.cmake file in the build directory, which is
# what is found by cmake in order to link to the Components library from its build directory,
# e.g. for when you're developing in parallel the Components library and an app which uses it.
###################################################################################################

export(TARGETS Components ${DEFINED_COMPONENTS} FILE ComponentsConfig.cmake)

###################################################################################################
# Here are the build rules for the overall library
###################################################################################################

# NOTE: these are not being compiled in because they currently have too many unmet dependencies
# and require a nontrivial amount of integration work.
# Please keep these alphabetized.
set(NON_COMPONENTIZED_CODE
    # Color.cpp # has other dependencies
    # Color.h
    # FrameBufferObject.cpp # depends on SimGL.h which has other dependencies
    # FrameBufferObject.h
    OculusVR.cpp
    OculusVR.h
    Primitive.cpp
    Primitive.h
    RenderBuffer.cpp
    RenderBuffer.h
    # SimGL.cpp # has other dependencies
    # SimGL.h
    Texture.cpp 
    Texture.h
    UtilGL.cpp
    UtilGL.h
    Utilities.h
)

# TODO: make some cmake messages that complain about files in the non-componentized-code directory,
# otherwise it could become a dumping ground like MathUtil or Utility.

###################################################################################################
# These Doxygen build rules were taken from 
# http://www.bluequartz.net/projects/EIM_Segmentation/SoftwareDocumentation/html/usewithcmakeproject.html
###################################################################################################

# Add an Option to toggle the generation of the API documentation.
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based (and potentially other format) API documentation" OFF)
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
    endif()
    # The DOXYGEN_OUTPUT_DIR var is used in Doxyfile.in to determine where to generate docs.
    set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc)
    # Configure the template Doxyfile for our specific project.
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    # Add a custom target to run Doxygen when ever the project is built.
    add_custom_target(
        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        SOURCES ${PROJECT_BINARY_DIR}/Doxyfile ${Components_SOURCES})
    # Define install rules for the documentation.
    # TODO: Ideally we would specify that the 'doc' target is a dependency of 'install'.
    install(DIRECTORY ${DOXYGEN_OUTPUT_DIR} DESTINATION ".")
endif()

return() # TEMP

# Define rules for creating a release distribution
define_install_rules(Components "${DEFINED_COMPONENTS}")

# TODO: write cpack rules -- it's probably an easier way to generate compiled lib packages
