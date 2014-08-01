cmake_minimum_required(VERSION 3.0)
project(oscontrols)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

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

add_subdirectory(contrib)
add_subdirectory(src)