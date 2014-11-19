list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# External module includes
include(ConditionalSources)
include(TargetImportedLibraries)

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
