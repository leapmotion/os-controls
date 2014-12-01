set(VRIntro_SOURCES
  main.cpp
  APIFrameSupplier.h
  APIFrameSupplier.cpp
  LeapListener.cpp
  LeapListener.h
)

option(VRIntro_SHOW_CONSOLE "Display console window for debugging" OFF)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
  set(_add_executable_options "MACOSX_BUNDLE")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wall -Werror -Wno-reorder -Wno-unused-variable -Wno-unused-private-field")

  # Locate all recourses

  set(VRINTRO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/source/VRIntro)
    
  file(GLOB COPY_IMG_RESOURCES  ${VRINTRO_SOURCE_DIR}/images/*)
  set_source_files_properties(
  ${COPY_IMG_RESOURCES}
  PROPERTIES
  MACOSX_PACKAGE_LOCATION Resources/images
  )
    
  file(GLOB COPY_COMPONENTS_GLSL_RESOURCES ${Components_DIR}/resources/*)
  file(GLOB COPY_VRINTRO_GLSL_RESOURCES ${VRINTRO_SOURCE_DIR}/shaders/*)
  set_source_files_properties(
    ${COPY_VRINTRO_GLSL_RESOURCES}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/shaders
  )
  set_source_files_properties(
    ${COPY_COMPONENTS_GLSL_RESOURCES}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
  )
endif()

if(WIN32)
  set(_add_executable_options WIN32)
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  # TODO: figure out how to enable these just for the targets defined within VRIntro.
  set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} /Zi /fp:fast /MP")
  set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} /fp:except /MP")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT /ignore:4099")
endif()

add_pch(VRIntro_SOURCES "stdafx.h" "stdafx.cpp")
add_executable(VRIntro 
  ${_add_executable_options} 
  ${VRIntro_SOURCES}
  ${COPY_IMG_RESOURCES}
  ${COPY_VRINTRO_GLSL_RESOURCES}
  ${COPY_COMPONENTS_GLSL_RESOURCES}
)
target_include_directories(VRIntro PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(VRIntro VRIntroLib)
target_package(VRIntro Leap)
set_property(TARGET VRIntro PROPERTY FOLDER "VRIntro")

###################################################################################################
# Resource rules
###################################################################################################
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  include(DefinePostBuildResourceCopyRules)
  define_post_build_resource_copy_rules(
    TARGET
      VRIntro
    RELATIVE_PATH_BASE
      ${EXECUTABLE_OUTPUT_PATH}
    RELATIVE_PATH_RESOURCES
      shaders/fractal-frag.glsl
      shaders/fractal-vert.glsl
      shaders/passthrough-frag.glsl
      shaders/passthrough-vert.glsl
      shaders/solid-frag.glsl
      shaders/solid-vert.glsl
      shaders/transparent-frag.glsl
      shaders/transparent-vert.glsl
      images/help.png
      images/lowfps.png
      images/no_oculus.png
      images/level3_popup.png
      images/level4_popup.png
      images/no_images.png
      images/random.png
      RunMirrored.bat
    ABSOLUTE_PATH_RESOURCES
      ${CMAKE_SOURCE_DIR}/src/components/library/Primitives/material-frag.glsl
      ${CMAKE_SOURCE_DIR}/src/components/library/Primitives/matrix-transformed-vert.glsl
      "${SDL_SHARED_LIB}"
  )
endif()