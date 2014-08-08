cmake_minimum_required(VERSION 3.0)
project(oscontrols)
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include(LeapCMakeTemplates)

leap_find_external_libraries(x86_vc12)
leap_use_standard_platform_settings()

include(TargetImportedLibraries)
include(AddMSVCPrecompiledHeader)
include(ConditionalSources)

enable_testing()

set(SFML_STATIC_LIBRARIES ON)
add_subdirectory(contrib)
add_subdirectory(src)
