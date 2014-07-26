cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".
project(Components)

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
    # Configure the template Doxyfile for our specific project.
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    # Add a custom target to run Doxygen when ever the project is built.
    add_custom_target(
        docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
endif()

###################################################################################################
# We use an EXTERNAL_LIBRARY_DIR variable as a hint to where all the libraries can be found.
# This is an optional means to not have to specify each library's root dir directly.
###################################################################################################

find_path(EXTERNAL_LIBRARY_DIR "glew-1.9.0" HINTS /opt/local/Libraries PATHS $ENV{PATH} $ENV{EXTERNAL_LIBRARY_DIR} NO_DEFAULT_PATH)

if(EXTERNAL_LIBRARY_DIR STREQUAL "EXTERNAL_LIBRARY_DIR-NOTFOUND")
  message(STATUS "External Library Directory not found, please specify a folder to look for external libraries")
  return()
endif()

###################################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake-modules")
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

include(ListSubdirectories)
include(TargetImportedLibraries) # for Walter's target_package command

###################################################################################################
# Component definitions
###################################################################################################

# This section defines what a component is and defines helper macros.

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
# set(COMPONENT_LIST "")

macro(begin_component_definitions)
    set(COMPONENT_LIST "")
endmacro()

macro(define_component NAME SOURCES EXPLICIT_COMPONENT_DEPENDENCIES EXPLICIT_LIBRARY_DEPENDENCIES)
    message("defining component \"${NAME}\" with:\n\tSOURCES = ${SOURCES}\n\tEXPLICIT_COMPONENT_DEPENDENCIES = ${EXPLICIT_COMPONENT_DEPENDENCIES}\n\tEXPLICIT_LIBRARY_DEPENDENCIES = ${EXPLICIT_LIBRARY_DEPENDENCIES}\n")
    set(COMPONENT_LIST ${COMPONENT_LIST} ${NAME})
    set(${NAME}_SOURCES ${SOURCES})
    set(${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES ${EXPLICIT_COMPONENT_DEPENDENCIES})
    set(${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES ${EXPLICIT_LIBRARY_DEPENDENCIES})
endmacro()

###################################################################################################
# This call initializes the COMPONENT_LIST var to empty -- defined components will be added to it.
# NOTE: Please keep the component definitions in alphabetical order by component name.
###################################################################################################

begin_component_definitions()

###################################################################################################
# The Application component provides a simple framework for an SDL-based application.
###################################################################################################

define_component(
    Application
    "Application.cpp;Application.h;EventHandler.h;Renderable.h;RenderableEventHandler.h"
    ""          # explicit component dependencies
    "SDL 2"     # explicit library dependencies
)

###################################################################################################
# The GLCompatibility component provides means for including OpenGL (and related) headers in
# a platform-agnostic way.
###################################################################################################

define_component(
    GLCompatibility
    "gl_glext_glu.h"
    ""              # explicit component dependencies
    "gl;glew;glu"   # explicit library dependencies
)

###################################################################################################
# The GLController component provides a single point of control for usage of OpenGL.
###################################################################################################

define_component(
    GLController
    "GLController.cpp;GLController.h"
    "GLCompatibility"   # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLShader component provides a C++ class which handles shaders -- compiling, binding, 
# unbinding, setting uniforms.
###################################################################################################

define_component(
    GLShader
    "GLShader.cpp;GLShader.h"
    "GLCompatibility"   # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLShaderLoader component provides ResourceManager-managed loading of GLShader via shader
# source files.
###################################################################################################

define_component(
    GLShaderLoader
    "GLShaderLoader.h"
    "GLShader;Resource;TextFileLoader"  # explicit component dependencies
    ""                                  # explicit library dependencies
)

###################################################################################################
# The GLTexture2 component provides a C++ class for managing 2D textures in OpenGL.
###################################################################################################

define_component(
    GLTexture2
    "GLTexture2.cpp;GLTexture2.h"
    "GLCompatibility"   # explicit component dependencies
    ""                  # explicit library dependencies
)

###################################################################################################
# The GLTexture2FreeImage component provides functionality for creating a GLTexture2 from
# an image file using the FreeImage library.
###################################################################################################

define_component(
    GLTexture2FreeImage
    "GLTexture2FreeImage.cpp;GLTexture2FreeImage.h"
    "GLTexture2"    # explicit component dependencies
    "FreeImage 3"   # explicit library dependencies
)

###################################################################################################
# The GLTexture2Loader component provides ResourceManager-managed loading of GLTexture2 objects
# using the FreeImage library.
###################################################################################################

define_component(
    GLTexture2Loader
    "GLTexture2Loader.cpp;GLTexture2Loader.h"
    "GLTexture2FreeImage;ResourceManager;SDLController" # explicit component dependencies
    ""                                                  # explicit library dependencies
)

###################################################################################################
# The Resource component provides convenient loading of ResourceManager-managed typed resources
# via a typed ResourceManager singleton.
###################################################################################################

define_component(
    Resource
    "Resource.h"
    "ResourceManager;Singleton" # explicit component dependencies
    ""                          # explicit library dependencies
)

###################################################################################################
# The ResourceManager component provides a means for loading and caching resources in a
# non-redundant way.
###################################################################################################

define_component(
    ResourceManager
    "ResourceManager.h"
    ""  # explicit component dependencies
    ""  # explicit library dependencies
)

###################################################################################################
# The SDLController component provides a single point of control for initializing, shutting down,
# and general usage of SDL.
###################################################################################################

define_component(
    SDLController
    "SDLController.cpp;SDLController.h"
    ""      # explicit component dependencies
    "SDL 2" # explicit library dependencies
)

###################################################################################################
# The Singleton component provides a simple mechanism for managing singleton C++ objects, via
# class inheritance or not.
###################################################################################################

define_component(
    Singleton
    "Singleton.h"
    ""  # explicit component dependencies
    ""  # explicit library dependencies
)

###################################################################################################
# The TextAndBinaryFile component provides simple C++ classes for loading and storing text and
# binary files (primarily intended to be used with Resource).
###################################################################################################

define_component(
    TextAndBinaryFile
    "TextFile.h"
    ""  # explicit component dependencies
    ""  # explicit library dependencies
)

###################################################################################################
# The TextAndBinaryFileLoader component provides ResourceManager-managed loading of text and
# binary files.
###################################################################################################

define_component(
    TextAndBinaryFileLoader
    "TextFileLoader.h"
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
    DataTypes.h
    # FrameBufferObject.cpp # depends on SimGL.h which has other dependencies
    # FrameBufferObject.h
    # GLBuffer.cpp # has other dependencies
    # GLBuffer.h
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

# Add the non-componentized code
# set(Components_SOURCES ${Components_SOURCES} ${NON_COMPONENTIZED_CODE})

# TODO: make some cmake messages that complain about files in the non-componentized-code directory,
# otherwise it could become a dumping ground like MathUtil or Utility.

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
target_package(Components Glew REQUIRED)
