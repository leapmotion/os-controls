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
    project(ComponentsSuite VERSION 0.4) # "Suite" meaning components library and example apps.
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})

    if(NOT _resetoncesentinel)
        set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "The friggin install prefix. geez" FORCE)
        set(_resetoncesentinel ON CACHE INTERNAL "Sentinel value to detect if we should override the install prefix on initial configuration.")
    endif()

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
endif()

# The subdirectory containing the Components library itself.
add_subdirectory(library)
# The subdirectories containing the example apps.
add_subdirectory(example_apps/BasicShapes)
add_subdirectory(example_apps/MediaControlSample)
add_subdirectory(example_apps/OculusSample)
add_subdirectory(example_apps/RiggedHandSample)
add_subdirectory(example_apps/Stub)
