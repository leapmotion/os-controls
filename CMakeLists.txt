cmake_minimum_required(VERSION 3.0)
project(oscontrols)
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

if(NOT (MSVC OR CMAKE_BUILD_TYPE))
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
endif()

if(APPLE)
  set(EXTERNAL_LIBRARY_DIR "/opt/local/Libraries")
  if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Mac OS X build architectures" FORCE)
  endif()
  if(NOT CMAKE_OSX_SYSROOT)
    set(CMAKE_OSX_SYSROOT "macosx10.8" CACHE STRING "Mac OS X build environment" FORCE)
  endif()
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7" CACHE STRING "Mac OS X deployment target" FORCE)
  mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

# Disable MinSizeRel & MaxSpeedRel
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Leap-Specific: We package any external libraries we're using in a single place, so add it to the default
#Search path. SDL-2.0.1 is used as a marker of something to look for.
find_path(EXTERNAL_LIBRARY_DIR "SDL-2.0.1"
  PATHS
    "$ENV{EXTERNAL_LIBRARY_DIR}"
    "$ENV{LIBRARIES_PATH}"
    "$ENV{PATH}"
    "/opt/local/Libraries"
  NO_DEFAULT_PATH)
list(INSERT CMAKE_PREFIX_PATH 0 "${EXTERNAL_LIBRARY_DIR}")

include(TargetImportedLibraries)
include(AddMSVCPrecompiledHeader)
include(ConditionalSources)

enable_testing()

add_subdirectory(contrib)
add_subdirectory(src)
