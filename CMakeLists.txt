cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

###################################################################################################
# High-level project definitions
###################################################################################################

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.8) # all glu functions are depricated in 10.9
project(Components VERSION 0.0)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

# TODO: set a default value for CMAKE_INSTALL_PREFIX to something like
# Components-versionnumber
# TODO: also look into cpack (for making Mac frameworks, Windows NSIS stuff, deb files, etc.)

###################################################################################################

# Module path and module includes
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)                # For AddSublibrary.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake-modules")   # For most other cmake modules

include(VerboseMessage)
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
# Sublibrary definitions
###################################################################################################

include(AddSublibrary)

# The added sublibrary target names will be accumulated in ADDED_SUBLIBRARIES

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(_cxx11_compiler_flags "-std=c++11")
endif()

add_sublibrary(
    C++11
    COMPILE_OPTIONS
        ${_cxx11_compiler_flags}
    BRIEF_DOC_STRING
        "A phony target which just provides compiler options for enabling C++11."
)

add_sublibrary(
    Application
    HEADERS
        Application.h
        EventHandler.h
        Renderable.h
        RenderableEventHandler.h
    SOURCES
        Application.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRING
        "A simple framework for an SDL-based application."
)

add_sublibrary(
    EigenTypes
    HEADERS
        EigenTypes.h
    EXPLICIT_LIBRARY_DEPENDENCIES
        "Eigen 3.2.1"
    BRIEF_DOC_STRING
        "Convenience typedefs for working with particular Eigen vector/matrix types."
)

add_sublibrary(
    Color
    HEADERS
        Color.h
    SOURCES
        Color.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        EigenTypes
    BRIEF_DOC_STRING
        "Provides RGBA color and some color-space conversions."
)

add_sublibrary(
    ResourceManager
    HEADERS
        ResourceManager.h
    BRIEF_DOC_STRING
        "A means for loading and caching resources in a non-redundant way."
)

# NOTE: MakeTransparent_Apple.mm is an Objective-C++ source file which should only be compiled
# on Apple platforms.  It should be ignored on other platforms.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(_maybe_MakeTransparent_Apple_mm "MakeTransparent_Apple.mm")
endif()

add_sublibrary(
    SDLController
    HEADERS
        SDLController.h
    SOURCES
        SDLController.cpp
        ${_maybe_MakeTransparent_Apple_mm}
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRING
        "A single point of control for init, shutdown, and general use of SDL."
)

add_sublibrary(
    SFMLController
    HEADERS
        SFMLController.h
    SOURCES
        SFMLController.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SFML 2.1"
    BRIEF_DOC_STRING
        "A single point of control for init, shutdown, and general use of SFML."
)
if(APPLE)
  set_property(SOURCE SFMLController/SFMLController.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
endif()

add_sublibrary(
    FbxSceneLoader
    HEADERS
        FbxSceneLoader.h
    SOURCES
        FbxSceneLoader.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        ResourceManager
        SDLController
    EXPLICIT_LIBRARY_DEPENDENCIES
        "FbxSdk" # TODO: specify version (IMPORTANT: inside the quotes, e.g. "FbxSdk 2015.1")
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of FbxScene objects using the FBX SDK."
)

add_sublibrary(
    GLCompatibility
    HEADERS
        gl_glext_glu.h
    EXPLICIT_LIBRARY_DEPENDENCIES
        "OpenGL 2.1"
        "Glew 1.9.0"
    BRIEF_DOC_STRING
        "A means for including OpenGL (and related) headers in a platform-agnostic way."
)

add_sublibrary(
    GLTexture2
    HEADERS
        GLTexture2.h
    SOURCES
        GLTexture2.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        GLCompatibility
    BRIEF_DOC_STRING
        "A C++ class for managing 2D textures in OpenGL."
)

add_sublibrary(
    FrameBufferObject
    HEADERS
        FrameBufferObject.h
        RenderBuffer.h
    SOURCES
        FrameBufferObject.cpp
        RenderBuffer.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2
        GLCompatibility
    BRIEF_DOC_STRING
        "Render to an OpenGL render buffer or texture."
)

add_sublibrary(
    GLBuffer
    HEADERS
        GLBuffer.h
    SOURCES
        GLBuffer.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLCompatibility
    BRIEF_DOC_STRING
        "A fast GPU buffer object for OpenGL geometry."
)

add_sublibrary(
    GLController
    HEADERS
        GLController.h
    SOURCES
        GLController.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLCompatibility
    BRIEF_DOC_STRING
        "A single point of control for usage of OpenGL."
)

add_sublibrary(
    GLMatrices
    HEADERS
        ModelViewProjection.h
    SOURCES
        ModelViewProjection.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        EigenTypes
        GLCompatibility
    BRIEF_DOC_STRING
        "Provides manipulation of modelview and projection matrices for usage in shaders."
)

add_sublibrary(
    GLShader
    HEADERS
        GLShader.h
    SOURCES
        GLShader.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        GLCompatibility
    BRIEF_DOC_STRING
        "A C++ class which manages GLSL-based shader programs."
)

add_sublibrary(
    Singleton
    HEADERS
        Singleton.h
    BRIEF_DOC_STRING
        "A simple mechanism for managing singleton C++ objects."
)

add_sublibrary(
    Resource
    HEADERS
        Resource.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        ResourceManager
        Singleton
    BRIEF_DOC_STRING
        "Convenient typed, ResourceManager-loaded resources via singleton."
)

add_sublibrary(
    TextAndBinaryFile
    HEADERS
        TextFile.h
    BRIEF_DOC_STRING
        "Simple C++ classes for loading/storing text/binary files."
)

add_sublibrary(
    TextAndBinaryFileLoader
    HEADERS
        TextFileLoader.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        ResourceManager
        SDLController
        TextAndBinaryFile
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of text and binary files."
)

add_sublibrary(
    GLShaderLoader
    HEADERS
        GLShaderLoader.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLShader
        Resource
        TextAndBinaryFileLoader
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of GLSL-based shader programs."
)

add_sublibrary(
    GLTexture2FreeImage
    HEADERS
        GLTexture2FreeImage.h
    SOURCES
        GLTexture2FreeImage.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2
    EXPLICIT_LIBRARY_DEPENDENCIES
        "FreeImage 3"
    BRIEF_DOC_STRING
        "Functionality for creating a GLTexture2 from an image file via FreeImage."
)

add_sublibrary(
    GLTexture2Loader
    HEADERS
        GLTexture2Loader.h
    SOURCES
        GLTexture2Loader.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2FreeImage
        ResourceManager
        SDLController
    BRIEF_DOC_STRING
        "Provides ResourceManager-managed loading of GLTexture2 objects using FreeImage."
)

add_sublibrary(
    SceneGraph
    HEADERS
        SceneGraphNode.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        EigenTypes
    BRIEF_DOC_STRING
        "Provides a C++ baseclass which implements the 'scene graph' design pattern (see Wikipedia)."
)

add_sublibrary(
    Primitives
    HEADERS
        PrimitiveBase.h
        PrimitiveGeometry.h
        Primitives.h
        RenderState.h
    SOURCES
        PrimitiveBase.cpp
        PrimitiveGeometry.cpp
        Primitives.cpp
        RenderState.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        Color
        GLBuffer
        GLMatrices
        SceneGraph
    BRIEF_DOC_STRING
        "Provides some simple shapes in a transform hierarchy."
)

add_sublibrary(
    LeapListener
    HEADERS
        LeapListener.h
    SOURCES
        LeapListener.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "Leap"
    BRIEF_DOC_STRING
        "Provides a small but useful extension of the Leap::Listener class."
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
# Global compiler options (and related) go here.  NOTE: ANY SUBLIBRARY-SPECIFIC COMPILER OPTIONS
# SHOULD BE MADE IN THE CORRESPONDING SUBLIBRARY DEFINITION!
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
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE clang_full_version_string)
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

add_library(Components INTERFACE)

###################################################################################################
# Add options for defining which sublibraries should be linked into the Components library, and
# link them.
###################################################################################################

# Construct the list of sublibraries that are required to be linked.
set(_explicit_link_sublibraries "")
foreach(_sublibrary ${ADDED_SUBLIBRARIES})
    list(APPEND _explicit_link_sublibraries ${_sublibrary})
    target_link_libraries(Components INTERFACE ${_sublibrary})
endforeach()

set(_temp "${ADDED_SUBLIBRARIES}")
list(SORT _temp)
set(SUBLIBRARIES_DEFINED_readonly ${_temp} CACHE STRING "This is the list of sublibraries that were successfully defined (all dependencies were met)." FORCE)

set(_temp "${UNADDED_SUBLIBRARIES}")
list(SORT _temp)
set(SUBLIBRARIES_UNDEFINED_readonly ${_temp} CACHE STRING "This is the list of sublibraries that were not successfully defined (some dependencies were unmet)." FORCE)

print_dependency_graph_of_sublibrary_linking_library(Components "${ADDED_SUBLIBRARIES}")

###################################################################################################
# Define install rules for the library.  This is probably not the legit way to do it, and we
# should figure out what the legit way is.
###################################################################################################

# Recursively determine all sublibrary dependencies implicitly enabled via the explicit enabling.
set(_all_link_sublibraries "")
foreach(_sublibrary ${_explicit_link_sublibraries})
    compute_all_sublibrary_dependencies_of(${_sublibrary} _sublibrary_deps)
    list(APPEND _all_link_sublibraries ${_sublibrary_deps}) # This may append redundant sublibraries.
endforeach()
# Sort and and remove duplicates, because we will iterate through the list, defining install
# rules for each, and we don't want to make redundant install rules.
list(SORT _all_link_sublibraries)
list(REMOVE_DUPLICATES _all_link_sublibraries)

# Define the install rules while accumulating include directories.
set(_Components_INCLUDE_DIRECTORIES "")
foreach(_sublibrary ${_all_link_sublibraries})
    get_target_property(_is_phony ${_sublibrary} INTERFACE_IS_PHONY)
    if(NOT _is_phony)
        get_target_property(_dir ${_sublibrary} INTERFACE_INCLUDE_DIRECTORIES)
        # message("making install rules for sublibrary ${_sublibrary}, include dir ${_dir}")
        list(APPEND _Components_INCLUDE_DIRECTORIES "include/${_dir}")
        get_target_property(_headers ${_sublibrary} INTERFACE_PATH_PREFIXED_HEADERS)
        install(
            FILES ${_headers}
            DESTINATION "include/${_sublibrary}"
        )
    endif()
endforeach()

# Determine all library dependencies (of linked sublibraries) for purposes of generating find_dependency
# commands in ComponentsConfig.cmake.
set(ALL_LIBRARY_DEPENDENCIES "")
foreach(_sublibrary ${_all_link_sublibraries})
    get_target_property(_explicit_library_dependencies ${_sublibrary} INTERFACE_EXPLICIT_LIBRARY_DEPENDENCIES)
    list(APPEND ALL_LIBRARY_DEPENDENCIES ${_explicit_library_dependencies})
endforeach()
list(SORT ALL_LIBRARY_DEPENDENCIES)
list(REMOVE_DUPLICATES ALL_LIBRARY_DEPENDENCIES)

install(
    TARGETS Components ${_all_link_sublibraries}
    EXPORT Components
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION ${_Components_INCLUDE_DIRECTORIES}
)
# This causes automatically-generated export targets to be defined during the install step in
#   ${CMAKE_INSTALL_PREFIX}/cmake/ComponentsExport.cmake
# which will be called by ${CMAKE_INSTALL_PREFIX}/ComponentsConfig.cmake (see next step with
# configure_file and install).
install(
    EXPORT Components
    NAMESPACE Components::
    DESTINATION cmake
    FILE ComponentsExport.cmake
)
# Generate the file ComponentsConfig.cmake that will be installed with the `make install` steps.
# NOTE: This is distinct from the ComponentsConfig.cmake file that will be generated in
# ${PROJECT_BINARY_DIR} by the export command below.
configure_file(${CMAKE_SOURCE_DIR}/cmake/ComponentsConfig.cmake.in ${PROJECT_BINARY_DIR}/cmake/for_install_dir/ComponentsConfig.cmake @ONLY)
# Install rule for the ComponentsConfig.cmake file just generated.  It goes in the root directory
# of the installation, and is the file that cmake looks for and uses in find_package.
install(
    FILES ${PROJECT_BINARY_DIR}/cmake/for_install_dir/ComponentsConfig.cmake
    DESTINATION .
)
# Generate the version configuration file.  This is the preferred method for indicating the presence
# of a library and conveying version information to the find_package cmake command.
configure_file(${CMAKE_SOURCE_DIR}/cmake/GenericConfigVersion.cmake.in ${PROJECT_BINARY_DIR}/ComponentsConfigVersion.cmake @ONLY)
# Install rule for the version configuration file.  It should go in the root dir of the install.
install(
    FILES ${PROJECT_BINARY_DIR}/ComponentsConfigVersion.cmake
    DESTINATION .
)

###################################################################################################
# Export rule for creating the ComponentsConfig.cmake file in the build directory, which is
# what is found by cmake in order to link to the Components library from its build directory,
# e.g. for when you're developing in parallel the Components library and an app which uses it.
###################################################################################################

# Generate the file ComponentsConfig.cmake that will be generated in the build dir by the build.
configure_file(${CMAKE_SOURCE_DIR}/cmake/ComponentsConfig.cmake.in ${PROJECT_BINARY_DIR}/ComponentsConfig.cmake @ONLY)
# Generate the file which defines all the targets of this project.
export(
    TARGETS
        Components
        ${ADDED_SUBLIBRARIES}
    NAMESPACE Components::
    FILE cmake/ComponentsExport.cmake
)

###################################################################################################
# Here are the build rules for the overall library
###################################################################################################

# NOTE: these are not being compiled in because they currently have too many unmet dependencies
# and require a nontrivial amount of integration work.
# Please keep these alphabetized.
set(NON_COMPONENTIZED_CODE
    OculusVR.cpp
    OculusVR.h
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
# Define the custom build and install rules for the documentation.
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
    endif()
    # The DOXYGEN_OUTPUT_DIR var is used in Doxyfile.in to determine where to generate docs.
    set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc)
    # Configure the template Doxyfile for our specific project.
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
    # Add a custom target to run Doxygen when ever the project is built.
    add_custom_target(
        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        SOURCES ${PROJECT_BINARY_DIR}/Doxyfile ${Components_SOURCES})
    # Define install rules for the documentation.
    # TODO: Ideally we would specify that the 'doc' target is a dependency of 'install'.
    install(DIRECTORY ${DOXYGEN_OUTPUT_DIR} DESTINATION ".")
endif()

# TODO: write cpack rules -- it's probably an easier way to generate compiled lib packages
