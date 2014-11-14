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
    project(Components VERSION 0.3)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})

    if(NOT _resetoncesentinel)
        set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "The friggin install prefix. geez" FORCE)
        set(_resetoncesentinel ON CACHE INTERNAL "Sentinel value to detect if we should override the install prefix on initial configuration.")
    endif()

    # TODO: set a default value for CMAKE_INSTALL_PREFIX to something like
    # Components-versionnumber
    # TODO: also look into cpack (for making Mac frameworks, Windows NSIS stuff, deb files, etc.)

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

# Module path and module includes
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)             # For DefinePostBuildResourceCopyRules.cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)                # For AddSublibrary.cmake
# Adding to CMAKE_MODULE_PATH is necessary to get the VerboseMessage cmake module.
# TODO: This can go away once the non-self-containment TODO in
# src/components/library/cmake/ComponentsConfig.cmake.in is resolved.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake-modules") # For most other cmake modules


###################################################################################################
# Sublibrary definitions
###################################################################################################

include(AddSublibrary)

# Add all subdirectories:
file(GLOB all_subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(subdirectory ${all_subdirectories})
  set(curdir "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}")
  if(NOT ${subdirectory} STREQUAL "." AND EXISTS "${curdir}/CMakeLists.txt")
    add_subdirectory(${subdirectory})
  endif()
endforeach()

###################################################################################################
# Global compiler options (and related) go here.  NOTE: ANY SUBLIBRARY-SPECIFIC COMPILER OPTIONS
# SHOULD BE MADE IN THE CORRESPONDING SUBLIBRARY DEFINITION!
###################################################################################################

# TODO: look into using target_compile_definitions and target_compile_options,
# though these seem sufficiently global to warrant the extreme measure of
# setting CMAKE_CXX_FLAGS.

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") # "GNU" is GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-reorder -Wno-unused-variable")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX") # /WX is to treat warnings as errors.
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    option(MSVC_USE_MT "Build using static runtime instead of dynamic" OFF)
    if (MSVC_USE_MT)
      string(REGEX REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
      string(REGEX REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    endif()
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
endif()

if (MSVC_USE_MT)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:LIBCMT")
endif()
