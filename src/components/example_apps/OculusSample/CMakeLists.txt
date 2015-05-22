set(_this_is_standalone_project FALSE)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(_this_is_standalone_project TRUE)
endif()

if(_this_is_standalone_project)
    cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

    project(OculusSample)

    # Enables structuring of project into separate folders (e.g. in MSVC, allowing the existence of an INSTALL project).
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake-modules")
    set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

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
else()
    set(Components_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../library")
endif()

###################################################################################################
# Definition of how to build the OculusSample app
###################################################################################################

set(OculusSample_SOURCES
    main.cpp
    OculusApplication.cpp
    OculusApplication.h
)

set(_add_executable_options "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(_add_executable_options "MACOSX_BUNDLE")
endif()

add_executable(OculusSample ${_add_executable_options} ${OculusSample_SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wall -Werror")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
endif()

if(_this_is_standalone_project)
    # Adding to CMAKE_MODULE_PATH is necessary to get the VerboseMessage cmake module.
    # TODO: This can go away once the non-self-containment TODO in
    # src/components/cmake/ComponentsConfig.cmake.in is resolved.
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake-modules")
    find_package(Components 0.5)
    find_package(SDL 2)
    find_package(Eigen 3.2.1)
    find_package(Glew 1.9.0)
    find_package(OpenGL 2.1) # It's necessary to link OpenGL after Glew on Linux.
    find_package(OculusSDK)
#     find_package(FreeImage 3)
    find_package(SFML 2.1)
#     find_package(Leap 2)
    target_link_libraries(OculusSample PUBLIC Components::Application Components::GLShaderLoader Components::LeapGLCore Components::OculusVR Components::Primitives Components::SFMLController)
else()
    set_property(TARGET OculusSample PROPERTY FOLDER "Components example apps")
    target_link_libraries(OculusSample PUBLIC Application GLShaderLoader LeapGLCore OculusVR Primitives SFMLController)
endif()

###################################################################################################
# Post-build resource-copying rules
###################################################################################################

# Adding to CMAKE_MODULE_PATH is necessary to get the DefinePostBuildResourceCopyRules cmake module.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
include(DefinePostBuildResourceCopyRules)

if(_this_is_standalone_project)
    define_post_build_resource_copy_rules(
        TARGET
            OculusSample
        RELATIVE_PATH_BASE
            ${EXECUTABLE_OUTPUT_PATH}
        ABSOLUTE_PATH_RESOURCES
            ${Components_DIR}/resources/material-frag.glsl
            ${Components_DIR}/resources/matrix-transformed-vert.glsl
    )
else()
    define_post_build_resource_copy_rules(
        TARGET
            OculusSample
        RELATIVE_PATH_BASE
            ${EXECUTABLE_OUTPUT_PATH}
        ABSOLUTE_PATH_RESOURCES
            ${Components_DIR}/Primitives/material-frag.glsl
            ${Components_DIR}/Primitives/matrix-transformed-vert.glsl
    )
endif()
