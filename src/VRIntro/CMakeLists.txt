list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# External module includes
include(ConditionalSources)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

include(TargetImportedLibraries) # for Walter's target_package command

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


# Adds a precompiled header
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  if(MSVC)
    set_source_files_properties(${PrecompiledSource}
        PROPERTIES
        COMPILE_FLAGS "/Yc${PrecompiledHeader}"
        )
    foreach( src_file ${${SourcesVar}} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yu${PrecompiledHeader}"
            )
    endforeach( src_file ${${SourcesVar}} )
    list(APPEND ${SourcesVar} ${PrecompiledHeader} ${PrecompiledSource})
  endif(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

# Run all of our project configuration steps
add_subdirectory(source)

# Include directory setup
set(INCLUDE_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source" CACHE PATH "Path to include files for VRIntro")

# Let everyone know we're here
include(CMakePackageConfigHelpers)
configure_package_config_file(
  VRIntroConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/VRIntroConfig.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/VRIntro/cmake
  PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/VRIntroVersion.cmake
                                 VERSION 1.0.0
                                 COMPATIBILITY SameMajorVersion )

# Add all targets to the build-tree export set
export(TARGETS VRIntroLib
  FILE "${PROJECT_BINARY_DIR}/VRIntroTargets.cmake")
