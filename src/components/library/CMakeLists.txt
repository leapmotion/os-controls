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

# The added sublibrary target names will be accumulated in ADDED_SUBLIBRARIES

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(_cxx11_compiler_flags "-std=c++11")
endif()

add_subdirectory(Application)
add_subdirectory(Animation)
add_subdirectory(EigenTypes) # TODO: this should probably be renamed to just Eigen
add_subdirectory(C++11)
add_subdirectory(Color)
add_subdirectory(External)
add_subdirectory(FbxSceneLoader)
add_subdirectory(FrameBufferObject)
add_subdirectory(GLCompatibility)
add_subdirectory(GLTexture2FreeImage)
add_subdirectory(GLTexture2Loader)
add_subdirectory(HandCursor)
add_subdirectory(GLTexture2)
add_subdirectory(GLBuffer)
add_subdirectory(GLVertexBuffer)
add_subdirectory(GLController)
add_subdirectory(GLMatrices)
add_subdirectory(GLShaderLoader)
add_subdirectory(GLShader)
add_subdirectory(GLMaterial)
add_subdirectory(LeapListener)
add_subdirectory(OculusVR)
add_subdirectory(Primitives)
add_subdirectory(RadialMenu)
add_subdirectory(Resource)
add_subdirectory(ResourceManager)
add_subdirectory(SDLController)
add_subdirectory(SceneGraph)
add_subdirectory(ScopeGuard)
add_subdirectory(SFMLController)
add_subdirectory(Singleton)
add_subdirectory(TextAndBinaryFile)
add_subdirectory(TextAndBinaryFileLoader)

###################################################################################################
# Loader path stuff (necessary for correct loading of dylibs when the dylib files are moved).
###################################################################################################

# TODO: make these into target properties.  BUILD_WITH_INSTALL_RPATH is a target property.
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# This specifies the directory prefix for the install name (see output of `otool -L libComponents.dylib`).
# Shared libraries should use "@loader_path", and can be installed side by side with the application
# binary.  See the Stub example app for usage.
set(CMAKE_INSTALL_NAME_DIR "@loader_path")

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
# Definition of the actual Components library -- this is the target that will be used by external
# projects that link this library.
###################################################################################################

if(_this_is_standalone_project)
    # TODO: We could build both static and shared libraries.  There will be a name collision in
    # the library target, which we can resolve using
    # http://datainfer.wordpress.com/2013/10/24/make-both-static-and-shared-libraries-in-one-build-with-cmake/
    # However I haven't implemented this directly because there is a different name collision
    # problem in multiple calls to target_package to specify dependencies (e.g. on Eigen).

    # TODO: could/should this be a target property?
    option(BUILD_SHARED_LIBS "Build Components as a shared library (instead of static)" OFF)
endif()

add_library(Components INTERFACE)

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
# Define install rules for the library.  This is probably not the legit way to do it, and we
# should figure out what the legit way is.
###################################################################################################

# Recursively determine all sublibrary dependencies implicitly enabled via the explicit enabling.
set(_all_link_sublibraries "")
foreach(_sublibrary ${_explicit_link_sublibraries})
    compute_all_sublibrary_dependencies_of(${_sublibrary} _sublibrary_deps)
    list(APPEND _all_link_sublibraries ${_sublibrary_deps}) # This may append redundant sublibraries.
endforeach()
# Sort and and remove duplicates, because we will iterate through the list, defining install
# rules for each, and we don't want to make redundant install rules.
list(SORT _all_link_sublibraries)
list(REMOVE_DUPLICATES _all_link_sublibraries)

# Define the install rules while accumulating include directories.
set(_Components_INCLUDE_DIRECTORIES "")
foreach(_sublibrary ${_all_link_sublibraries})
    get_target_property(_is_phony ${_sublibrary} INTERFACE_IS_PHONY)
    if(NOT _is_phony)
        get_target_property(_dir ${_sublibrary} INTERFACE_INCLUDE_DIRECTORIES)
        # message("making install rules for sublibrary ${_sublibrary}, include dir ${_dir}")
        list(APPEND _Components_INCLUDE_DIRECTORIES "include/${_dir}")
        get_target_property(_source_path ${_sublibrary} INTERFACE_SOURCE_PATH)
        get_target_property(_headers ${_sublibrary} INTERFACE_PATH_PREFIXED_HEADERS)
        install(
            FILES ${_headers}
            DESTINATION "include/${_source_path}"
            COMPONENT "Components"
        )
        get_target_property(_resources ${_sublibrary} INTERFACE_PATH_PREFIXED_RESOURCES)
        install(
            FILES ${_resources}
            DESTINATION "resources"
            COMPONENT "Components"
        )
    endif()
endforeach()

# Determine all library dependencies (of linked sublibraries) for purposes of generating find_dependency
# commands in ComponentsConfig.cmake.
set(ALL_LIBRARY_DEPENDENCIES "")
foreach(_sublibrary ${_all_link_sublibraries})
    get_target_property(_explicit_library_dependencies ${_sublibrary} INTERFACE_EXPLICIT_LIBRARY_DEPENDENCIES)
    list(APPEND ALL_LIBRARY_DEPENDENCIES ${_explicit_library_dependencies})
endforeach()
list(SORT ALL_LIBRARY_DEPENDENCIES)
list(REMOVE_DUPLICATES ALL_LIBRARY_DEPENDENCIES)

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

# TODO: write cpack rules -- it's probably an easier way to generate compiled lib packages

###################################################################################################
# Unit test components and rules
###################################################################################################

add_sublibrary(
    GTest
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        External/GTest
    HEADERS
        gtest/gtest.h
    SOURCES
        gtest/gtest-all.cc
        gtest/gtest_main.cc
    COMPILE_DEFINITIONS
        GTEST_HAS_PTHREAD=0 # This is specifically for Linux -- threads are not necessary.
    BRIEF_DOC_STRING
        "Provides an easy way to include and link the Google Test framework."
)

add_sublibrary(
    GLTestFramework
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        Test/GLTestFramework
    HEADERS
        GLTestFramework.h
    SOURCES
        GLTestFramework.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLController
        GTest
        SDLController
    BRIEF_DOC_STRING
        "Provides an easy way to create GL-based tests."
)

# This effectively resets ADDED_SUBLIBRARIES and UNADDED_SUBLIBRARIES.  It is intentionally
# called after the call to add_sublibrary(GTest ...) so that only the unit test components
# show up in ADDED_SUBLIBRARIES.
begin_sublibrary_definitions()

add_sublibrary(
    C++11Test
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        C++11/Test
    SOURCES
        C++11ComplianceTest.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        GTest
    BRIEF_DOC_STRING
        "Unit tests for the C++11 component."
)

add_sublibrary(
    EigenTest
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        EigenTypes/Test
    SOURCES
        EigenTest.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        EigenTypes
        GTest
    BRIEF_DOC_STRING
        "Unit tests for the EigenTypes component." # TODO: EigenTypes should be renamed to Eigen
)

add_sublibrary(
    GLShaderTest
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        GLShader/Test
    SOURCES
        GLShaderTest.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLShaderLoader
        GLTestFramework
    BRIEF_DOC_STRING
        "Unit tests for the GLShader component."
)

add_sublibrary(
    GLTexture2Test
    SOURCE_PATH
        GLTexture2/Test
    SOURCES
        GLTexture2Test.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2
        GLTestFramework
    BRIEF_DOC_STRING
        "Unit tests for the GLTexture2 component."
)

add_sublibrary(
    SceneGraphTest
    SOURCE_PATH
        SceneGraph/Test
    SOURCES
        SceneGraphNodePropertiesTest.cpp
        SceneGraphNodeTest.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GTest
        SceneGraph
    BRIEF_DOC_STRING
        "Unit tests for the GLTexture2 component."
)

set(_tests ${ADDED_SUBLIBRARIES})

# In order for gtest to "see" the unit tests, the sources apparently have to be linked
# directly into the test executable.  The call to target_link_libraries seems redundant
# and also seems like it would cause duplicate symbol linker errors, but the linker
# effectively throws out the entire contents of the linked components.  The call to
# target_link_libraries is what propagates all the necessary include directories and
# libraries to link into the executable.  This sort of hack-ish solution allows unit
# tests to be organized in the "sublibrary" paradigm as the other components are.
# NOTE: Because of this hack, the unit test sources will be built twice (once to build
# the unit test component targets, and a second time to build the ComponentsTest executable).
set(ComponentsTest_SOURCES "")
foreach(_test ${_tests})
    get_target_property(_headers ${_test} INTERFACE_PATH_PREFIXED_HEADERS)
    get_target_property(_sources ${_test} INTERFACE_PATH_PREFIXED_SOURCES)
    set_property(TARGET ${_test} PROPERTY FOLDER "Tests")
    list(APPEND ComponentsTest_SOURCES ${_headers} ${_sources})
endforeach()
add_executable(ComponentsTest ${ComponentsTest_SOURCES})

if(NOT _this_is_standalone_project)
    set_property(TARGET ComponentsTest PROPERTY FOLDER "Components")
endif()

if (MSVC_USE_MT)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:LIBCMT")
endif()

# Link each of the dependencies of each unit test components.  The reason for this
# indirection is because the test sources must be specified directly in the call to
# add_executable above.  This is a totally suboptimal solution in terms of cmake
# code duplication and fragility but avoids redundant compilation.
# The alternative which is a tradeoff between simplicity and technical incorrectness is:
# target_link_libraries(ComponentsTest PUBLIC ${_tests})
foreach(_test ${_tests})
    # Set compile definitions and options.
    get_target_property(_compile_definitions ${_test} INTERFACE_COMPILE_DEFINITIONS)
    if(_compile_definitions)
        target_compile_definitions(ComponentsTest PUBLIC ${_compile_definitions})
    endif()
    get_target_property(_compile_options ${_test} INTERFACE_COMPILE_OPTIONS)
    if(_compile_options)
        target_compile_options(ComponentsTest PUBLIC ${_compile_options})
    endif()
    # Link the explicit sublibrary dependencies of each test
    get_target_property(_explicit_sublibrary_dependencies ${_test} INTERFACE_EXPLICIT_SUBLIBRARY_DEPENDENCIES)
    foreach(_dep ${_explicit_sublibrary_dependencies})
        target_link_libraries(ComponentsTest PUBLIC ${_dep})
    endforeach()
    # Link the explicit library dependencies of each test
    get_target_property(_explicit_library_dependencies ${_test} INTERFACE_EXPLICIT_LIBRARY_DEPENDENCIES)
    foreach(_dep ${_explicit_library_dependencies})
        string(REPLACE " " ";" _semicolon_delimited_dep ${_dep})
        list(GET _semicolon_delimited_dep 0 _lib_name)
        set(_lib_target_name ${_lib_name}::${_lib_name})
        target_link_libraries(ComponentsTest PUBLIC ${_lib_target_name})
    endforeach()
endforeach()
