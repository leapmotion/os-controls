set(_this_is_standalone_project FALSE)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(_this_is_standalone_project TRUE)
endif()

if(_this_is_standalone_project)
    cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

    project(RiggedHandSample)

    # Enables structuring of project into separate folders (e.g. in MSVC, allowing the existence of an INSTALL project).
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

    ###################################################################################################
    # We use an EXTERNAL_LIBRARY_DIR variable as a hint to where all the libraries can be found.
    # This is an optional means to not have to specify each library's root dir directly.
    ###################################################################################################

    find_path(EXTERNAL_LIBRARY_DIR "glew-1.9.0" HINTS /opt/local/Libraries PATHS $ENV{PATH} $ENV{EXTERNAL_LIBRARY_DIR} NO_DEFAULT_PATH)

    # TODO: Make EXTERNAL_LIBRARY_DIR detection optional, since users may not have their libraries
    # installed the same way we (Leap) do.
    if(EXTERNAL_LIBRARY_DIR STREQUAL "EXTERNAL_LIBRARY_DIR-NOTFOUND")
        message(STATUS "External Library Directory not found, optionally specify a folder to look for external libraries")
    else()
        # CMAKE_PREFIX_PATH is the path used for searching by FIND_XXX(), with appropriate suffixes added.
        # EXTERNAL_LIBRARY_DIR is a hint for all the find_library calls.
        list(INSERT CMAKE_PREFIX_PATH 0 ${EXTERNAL_LIBRARY_DIR})
    endif()
else()
    set(Components_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../library")
endif()

###################################################################################################
# Definition of how to build the RiggedHandSample app
###################################################################################################

set(RiggedHandSample_SOURCES
    main.cpp
    RiggedHandApplication.cpp
    RiggedHandApplication.h
    RiggedHandLayer.cpp
    RiggedHandLayer.h
)

set(_add_executable_options "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(_add_executable_options "MACOSX_BUNDLE")
endif()

add_executable(RiggedHandSample ${_add_executable_options} ${RiggedHandSample_SOURCES})

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
    # The definition of Components::* targets and automatic finding and linking of all library
    # dependencies is all accomplished via the ComponentsConfig.cmake file (in the root build dir
    # or root install dir of the Components package).
    find_package(Components)
    # The target_link_components function finds and links exactly the minimal set of libraries
    # needed by the specified components.
    target_link_components(RiggedHandSample PUBLIC Application GLController GLShaderLoader GLTexture2Loader LeapListener Primitives SFMLController RiggedHand)
else()
    set_property(TARGET RiggedHandSample PROPERTY FOLDER "Components example apps")
    target_link_libraries(RiggedHandSample PUBLIC Application GLController GLShaderLoader GLTexture2Loader LeapListener Primitives SFMLController Rigging RiggedHand)
endif()

###################################################################################################
# Post-build resource-copying rules
###################################################################################################

# Adding to CMAKE_MODULE_PATH is necessary to get the DefinePostBuildResourceCopyRules cmake module.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
include(DefinePostBuildResourceCopyRules)

if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/models)
  #TODO: Use ExternalProject_Add to grab these from a separate repository.
  message(WARNING "Rigged Hand Sample missing models & texture data.  Get them from \\\\Ocuserv\\Common\\toRaffi\\riggedHand")
endif()

define_post_build_resource_copy_rules(
    TARGET
        RiggedHandSample
    RELATIVE_PATH_BASE
        ${EXECUTABLE_OUTPUT_PATH}
    RELATIVE_PATH_RESOURCES
        shaders/lighting-frag.glsl
        shaders/lighting-vert.glsl
        models/Male_Rigged_Arm.FBX
        models/Female_Rigged_Arm.FBX
        textures/Male_Arm_DarkSkin_color.png
        textures/Male_Arm_DarkSkin_normal.png
        textures/Male_Arm_DarkSkin_specular.png
        textures/Male_Arm_LightSkin_color.png
        textures/Male_Arm_LightSkin_normal.png
        textures/Male_Arm_LightSkin_specular.png
        textures/Male_Arm_MediumSkin_color.png
        textures/Male_Arm_MediumSkin_normal.png
        textures/Male_Arm_MediumSkin_specular.png
        textures/Female_Arm_DarkSkin_color.png
        textures/Female_Arm_DarkSkin_normal.png
        textures/Female_Arm_DarkSkin_specular.png
        textures/Female_Arm_LightSkin_color.png
        textures/Female_Arm_LightSkin_normal.png
        textures/Female_Arm_LightSkin_specular.png
        textures/Female_Arm_MediumSkin_color.png
        textures/Female_Arm_MediumSkin_normal.png
        textures/Female_Arm_MediumSkin_specular.png
)

if(_this_is_standalone_project)
    define_post_build_resource_copy_rules(
        TARGET
            RiggedHandSample
        RELATIVE_PATH_BASE
            ${EXECUTABLE_OUTPUT_PATH}
        ABSOLUTE_PATH_RESOURCES
            ${Components_DIR}/resources/material-frag.glsl
            ${Components_DIR}/resources/matrix-transformed-vert.glsl
    )
else()
    # This is a temporary hack to get all of the post-build resource-copying rules to work
    # in the integrated-into-everything build.
    define_post_build_resource_copy_rules(
        TARGET
            RiggedHandSample
        RELATIVE_PATH_BASE
            ${EXECUTABLE_OUTPUT_PATH}
        ABSOLUTE_PATH_RESOURCES
            ${Components_DIR}/GLMaterial/material-frag.glsl
            ${Components_DIR}/GLMaterial/matrix-transformed-vert.glsl
    )
endif()
