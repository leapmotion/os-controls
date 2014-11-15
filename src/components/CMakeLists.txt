set(_this_is_standalone_project FALSE)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(_this_is_standalone_project TRUE)
endif()

if(_this_is_standalone_project)
    cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

    ###################################################################################################
    # High-level project definitions
    ###################################################################################################

    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.8) # all glu functions are depricated in 10.9
    project(ComponentsSuite VERSION 0.3) # "Suite" meaning components library and example apps.

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake-modules") # For most other cmake modules

    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})

    if(NOT _resetoncesentinel)
        set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "The friggin install prefix. geez" FORCE)
        set(_resetoncesentinel ON CACHE INTERNAL "Sentinel value to detect if we should override the install prefix on initial configuration.")
    endif()

    include(LeapCMakeTemplates)
    leap_find_external_libraries()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") #For our specialized cmake modules

# The subdirectory containing the Components library itself.
add_subdirectory(library)
# The subdirectories containing the example apps.
add_subdirectory(example_apps/BasicShapes)
add_subdirectory(example_apps/Stub)
