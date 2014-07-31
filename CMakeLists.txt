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
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}                     # For DefineComponents.cmake
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake-modules") # For most other cmake modules
include(ListSubdirectories)
include(TargetImportedLibraries) # for Walter's target_package command

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
# This call initializes the COMPONENTS var to empty -- defined components will be added to it.
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
# The Application component provides a simple framework for an SDL-based application.
###################################################################################################

define_component(
    Application
    "Application.h;EventHandler.h;Renderable.h;RenderableEventHandler.h"
    "Application.cpp"   # sources which won't be installed
    ""                  # non-sources to install
    ""                  # explicit component dependencies
    "SDL 2"             # explicit library dependencies
)


###################################################################################################
# The EigenTypes component provides convenience typedefs for working with Eigen matrices.
###################################################################################################

define_component(
    EigenTypes            
    "EigenTypes.h"      # sources to install
    ""                  # sources which won't be installed
    ""                  # non-sources to install
    ""                  # explicit component dependencies
    "Eigen"             # explicit library dependencies
)

###################################################################################################
# The FbxSceneLoader component provides ResourceManager-managed loading of FbxScene objects
# using the FBX SDK library.
###################################################################################################

define_component(
    FbxSceneLoader
    "FbxSceneLoader.h"                  # sources to install
    "FbxSceneLoader.cpp"                # sources which won't be installed
    ""                                  # non-sources to install
    "ResourceManager;SDLController"     # explicit component dependencies
    "FbxSdk"                            # explicit library dependencies
)

###################################################################################################
# The FrameBufferObject component provides a way to render to a render buffer or texture
###################################################################################################

define_component(
    FrameBufferObject            
    "FrameBufferObject.h;RenderBuffer.h"        # sources to install
    "FrameBufferObject.cpp;RenderBuffer.cpp"    # sources which won't be installed
    ""                                          # non-sources to install
    "GLTexture2;GLCompatibility"                # explicit component dependencies
    ""                                          # explicit library dependencies
)

###################################################################################################
# The GLBuffer component provides a fast GPU buffer object for geometry.
###################################################################################################

define_component(
    GLBuffer            
    "GLBuffer.h"        # sources to install
    "GLBuffer.cpp"      # sources which won't be installed
    ""                  # non-sources to install
    "GLCompatiblity"    # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLCompatibility component provides means for including OpenGL (and related) headers in
# a platform-agnostic way.
###################################################################################################

define_component(
    GLCompatibility
    "gl_glext_glu.h"    # sources to install
    ""                  # sources which won't be installed
    ""                  # non-sources to install
    ""                  # explicit component dependencies
    "gl;glew;glu"       # explicit library dependencies
)

###################################################################################################
# The GLController component provides a single point of control for usage of OpenGL.
###################################################################################################

define_component(
    GLController
    "GLController.h"    # sources to install
    "GLController.cpp"  # sources which won't be installed
    ""                  # non-sources to install
    "GLCompatibility"   # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLShader component provides a C++ class which handles shaders -- compiling, binding, 
# unbinding, setting uniforms.
###################################################################################################

define_component(
    GLShader
    "GLShader.h"        # sources to install
    "GLShader.cpp"      # sources which won't be installed
    ""                  # non-sources to install
    "GLCompatibility"   # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLShaderLoader component provides ResourceManager-managed loading of GLShader via shader
# source files.
###################################################################################################

define_component(
    GLShaderLoader
    "GLShaderLoader.h"                  # sources to install
    ""                                  # sources which won't be installed
    ""                                  # non-sources to install
    "GLShader;Resource;TextFileLoader"  # explicit component dependencies
    ""                                  # explicit library dependencies
)

###################################################################################################
# The GLTexture2 component provides a C++ class for managing 2D textures in OpenGL.
###################################################################################################

define_component(
    GLTexture2
    "GLTexture2.h"      # sources to install
    "GLTexture2.cpp"    # sources which won't be installed
    ""                  # non-sources to install
    "GLCompatibility"   # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLTexture2FreeImage component provides functionality for creating a GLTexture2 from
# an image file using the FreeImage library.
###################################################################################################

define_component(
    GLTexture2FreeImage
    "GLTexture2FreeImage.h"     # sources to install
    "GLTexture2FreeImage.cpp"   # sources which won't be installed
    ""                          # non-sources to install
    "GLTexture2"                # explicit component dependencies
    "FreeImage 3"               # explicit library dependencies
)

###################################################################################################
# The GLTexture2Loader component provides ResourceManager-managed loading of GLTexture2 objects
# using the FreeImage library.
###################################################################################################

define_component(
    GLTexture2Loader
    "GLTexture2Loader.h"                                # sources to install
    "GLTexture2Loader.cpp"                              # sources which won't be installed
    ""                                                  # non-sources to install
    "GLTexture2FreeImage;ResourceManager;SDLController" # explicit component dependencies
    ""                                                  # explicit library dependencies
)

###################################################################################################
# The Primitives component provides some simple shapes in a transform hierarchy.
###################################################################################################

define_component(
    Primitives
    "Primitive.h;Rectangle.h"     # sources to install
    "Primitive.cpp;Rectangle.cpp" # sources which won't be installed
    ""                            # non-sources to install
    "EigenTypes"                  # explicit component dependencies
    ""                            # explicit library dependencies
)

###################################################################################################
# The Resource component provides convenient loading of ResourceManager-managed typed resources
# via a typed ResourceManager singleton.
###################################################################################################

define_component(
    Resource
    "Resource.h"                # sources to install
    ""                          # sources which won't be installed
    ""                          # non-sources to install
    "ResourceManager;Singleton" # explicit component dependencies
    ""                          # explicit library dependencies
)

###################################################################################################
# The ResourceManager component provides a means for loading and caching resources in a
# non-redundant way.
###################################################################################################

define_component(
    ResourceManager
    "ResourceManager.h" # sources to install
    ""                  # sources which won't be installed
    ""                  # non-sources to install
    ""                  # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The SDLController component provides a single point of control for initializing, shutting down,
# and general usage of SDL.
###################################################################################################

# NOTE: MakeTransparent_Apple.mm is an Objective-C++ source file which should only be compiled
# on Apple platforms.  It should be ignored on other platforms.
define_component(
    SDLController
    "SDLController.h"                               # sources to install
    "MakeTransparent_Apple.mm;SDLController.cpp"    # sources which won't be installed
    ""                                              # non-sources to install
    ""                                              # explicit component dependencies
    "SDL 2"                                         # explicit library dependencies
    # TODO: require the AppKit framework (for NSOpenGLContext)
)

###################################################################################################
# The Singleton component provides a simple mechanism for managing singleton C++ objects, via
# class inheritance or not.
###################################################################################################

define_component(
    Singleton
    "Singleton.h"       # sources to install
    ""                  # sources which won't be installed
    ""                  # non-sources to install
    ""                  # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The TextAndBinaryFile component provides simple C++ classes for loading and storing text and
# binary files (primarily intended to be used with Resource).
###################################################################################################

define_component(
    TextAndBinaryFile
    "TextFile.h"        # sources to install
    ""                  # sources which won't be installed
    ""                  # non-sources to install
    ""                  # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The TextAndBinaryFileLoader component provides ResourceManager-managed loading of text and
# binary files.
###################################################################################################

define_component(
    TextAndBinaryFileLoader
    "TextFileLoader.h"                                  # sources to install
    ""                                                  # sources which won't be installed
    ""                                                  # non-sources to install
    "ResourceManager;SDLController;TextAndBinaryFile"   # explicit component dependencies
    ""                                                  # explicit library dependencies
)

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

# TODO: write functinos for the following:
# - Determining all component dependencies of a component (recursively).
# - Determining all library dependencies of a component (recursively).
# - Determining the include directories for a component.
# - Determining the library link options for a component.
# - Calling the appropriate function for defining a library for the component.

set(Components_SOURCES "")
set(Components_INCLUDE_DIRECTORIES "")
foreach(COMPONENT ${COMPONENTS})
    # message("adding sources for component ${COMPONENT}")
    foreach(SOURCE_FILE ${${COMPONENT}_SOURCES})
        set(SOURCE_PATH ${COMPONENT}/${SOURCE_FILE})
        # message("    adding ${SOURCE_PATH}")        
        set(Components_SOURCES ${Components_SOURCES} ${SOURCE_PATH})
    endforeach()
    # message("adding include directory for component ${COMPONENT}")
    set(Components_INCLUDE_DIRECTORIES ${Components_INCLUDE_DIRECTORIES} ${COMPONENT})
endforeach()

# Add the non-componentized code
# set(Components_SOURCES ${Components_SOURCES} ${NON_COMPONENTIZED_CODE})

# TODO: make some cmake messages that complain about files in the non-componentized-code directory,
# otherwise it could become a dumping ground like MathUtil or Utility.

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
# The Components library (static and/or shared) is defined here, along with library dependencies.
###################################################################################################

# TODO: We should build both static and shared libraries.  There will be a name collision in
# the library target, which we can resolve using
# http://datainfer.wordpress.com/2013/10/24/make-both-static-and-shared-libraries-in-one-build-with-cmake/
# However I haven't implemented this directly because there is a different name collision
# problem in multiple calls to target_package to specify dependencies (e.g. on Eigen).

# TODO: could/should this be a target property?
option(BUILD_SHARED_LIBS "Build Components as a shared library (instead of static)" OFF)

# Define the "Components" target -- it's a library having the specified sources.
add_library(Components ${Components_SOURCES})
# TEMP: for now, just link all library dependencies.  TODO: determine the libraries to link
# based on the desired set of components.

# Please keep these in alphabetical order.
target_package(Components Eigen 3.1.2 REQUIRED)
target_package(Components FbxSdk REQUIRED)
target_package(Components FreeImage REQUIRED)
target_package(Components Glew 1.9.0 REQUIRED)
target_package(Components OpenGL REQUIRED)
target_package(Components SDL 2 REQUIRED)
# This appears to be necessary (to build the Components library as a shared library on Mac).
# I'm not sure why it's different than the others.
target_link_libraries(Components ${OPENGL_gl_LIBRARY})

# Define rules for creating a release distribution
define_install_rules(Components "${COMPONENTS}")

###################################################################################################
# Include directories and compiler flags
###################################################################################################

# message("Components_INCLUDE_DIRECTORIES = ${Components_INCLUDE_DIRECTORIES}")
# message("Components_SOURCES = ${Components_SOURCES}")

# TEMP: for now, just include all include directories
foreach(_include_directory ${Components_INCLUDE_DIRECTORIES})
    target_include_directories(Components PRIVATE ${_include_directory})
    target_include_directories(Components INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_include_directory}>)
endforeach()

# TODO: use target_compile_definitions and target_compile_options
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # This is the correct way to detect the Clang compiler -- see http://www.cmake.org/cmake/help/v3.0/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_<LANG>_COMPILER_ID
    # TODO: there's probably some "real" way to get the compiler version,
    # or really just handle the -Wno-deprecated-register option more gracefully.
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
    string(REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
    # message("CLANG_VERSION_STRING = ${CLANG_VERSION_STRING}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall -Werror")
    if (CLANG_VERSION_STRING VERSION_GREATER 4.2)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")
    endif()
endif() # TODO: GCC ("GNU"), MSVC

# Some debug spew
# include(PrintTargetProperties)
# echo_target(Components)

# This command causes a ComponentsConfig.cmake file to be generated, which defines
# the cmake target properties for the Components target for use by external apps,
# but only for when you want to include/link this library from the build dir.
# NOTE: The file generated by this is NOT intended to be installed, and cmake is 
# supposed to not install it.
export(TARGETS Components FILE ComponentsConfig.cmake)

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

###################################################################################################
# CPack rules for automatic package generation -- just invoke `cpack` in the build dir.
###################################################################################################

# include(CPack)

# # set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Components")
# set(CPACK_PACKAGE_VENDOR "Leap Motion, Inc.")
# # set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# set(CPACK_PACKAGE_VERSION_MAJOR "0")
# set(CPACK_PACKAGE_VERSION_MINOR "0")
# set(CPACK_PACKAGE_VERSION_PATCH "1")
# # set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
# # if(WIN32 AND NOT UNIX)
# #   There is a bug in NSI that does not handle full unix paths properly. Make
# #   sure there is at least one set of four (4) backlasshes.
# #   set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
# #   set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
# #   set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
# #   set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
# #   set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
# #   set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
# #   set(CPACK_NSIS_MODIFY_PATH ON)
# # else(WIN32 AND NOT UNIX)
# #   set(CPACK_STRIP_FILES "bin/MyExecutable")
# #   set(CPACK_SOURCE_STRIP_FILES "")
# # endif(WIN32 AND NOT UNIX)
# # set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

