#.rst
# Components
# ------------
#
# Created by Victor Dods
# Functions and macros which assist in defining "components" (here, "components" has a meaning
# distinct from the one used generally in cmake).
#
# A "component" can be thought of as a "sub-library" (in the sense that it is a small library and
# is subordinate to the whole library).  A component satisfies two requirements:
# - Has a well-defined purpose, scope, and feature set.
# - Has well-defined dependencies, which are explicitly declared and are minimal.
#
# What is a good criteria for deciding how to group source code into components?
# Consider the set of all reasonable (non-contrived) applications that may use the
# code -- think of each source file to be a point in a plane.  Each application is
# going to use some subset of that source code, which corresponds to some subset
# of points in the plane.  For a given source file X, consider the set A of all
# applications that use it, and take the intersection I of the sets of source code
# that each application in A uses.  This will be some subset which contains X.  The
# component that X should belong to should contain exactly the set of source code I,
# which may contain more than just X.  TODO: visual example
#
# This is the list of defined components.  The component name should be identical to
# the subdirectory which contains all its source files.  A component name should be
# a C identifier that is WordCapitalized.  Each component should have
# the following macros defined.
#   Foo_SOURCES                         The list of source files for component "Foo"
#   Foo_INSTALL_FILES                   The list of files to copy into a "release" archive.
#   Foo_EXPLICIT_COMPONENT_DEPENDENCIES The list of components which component "Foo" explicitly depends on; not,
#                                       for example, components which "Foo" depends on through other components.
#                                       It's ok if there are redundancies here.
#   Foo_EXPLICIT_LIBRARY_DEPENDENCIES   The list of library-version pairs which component "Foo" explicity depends
#                                       on; not, for example, libraries which "Foo" depends on through other
#                                       components or libraries.  It's ok if there are redundancies here.
# The total component dependencies of a component can be determined recursively using these
# definitions.  Similarly, the total library dependencies of a component can be determined.

# Design notes for components
# ---------------------------
# - Each component should be able to be compiled as a library, thereby enforcing the strict
#   modularization which defines it.  If a component were compiled into a library, its component
#   dependencies would have to be either
#   * Compiled in, or
#   * linked as libraries themselves.
# - A compiled-and-minimal distribution of a component or set of components should be layed out as follows:
#   * DIST_DIR/doc/                                     # Dir for all documentation
#   * DIST_DIR/include/                                 # This is the include dir for everything
#   * DIST_DIR/include/Component1/something.h
#   * DIST_DIR/include/Component1/otherthing.h
#   * DIST_DIR/include/Component2/foo.h
#   * DIST_DIR/lib/                                     # Dir for all the lib files (static and dynamic)
#   * DIST_DIR/lib/libComponent1.a
#   * DIST_DIR/lib/libComponent1.so
#   * DIST_DIR/lib/libComponent2.a
#   * DIST_DIR/lib/libComponent2.so
#   * DIST_DIR/resources/                               # Dir for non-compiled, non-header files, e.g. shaders.
#   * DIST_DIR/resources/fancy-shader.glsl
#   * DIST_DIR/resources/pumpkin.png
#   * DIST_DIR/ComponentsConfig.cmake                   # Used by cmake projects that link to Components.
# - The components library should also be usable from its source dir, rather than only from
#   a packaged distribution.  This should be done by creating a ComponentsConfig.cmake file
#   in the build dir which sets the include dirs (from ${CMAKE_CURRENT_SOURCE_DIR}/source)
#   and the linking libraries (from the build dir).

function(print_value_of VAR_NAME INDENT)
    message("${INDENT}${VAR_NAME} = ${${VAR_NAME}}")
endfunction()

# DEFINED_COMPONENTS is a list of strings enumerating all the defined components.  Defining a
# component should append to it -- the define_component macro is a convience macro for defining
# all the correct variables for a component.
macro(begin_component_definitions)
    set(DEFINED_COMPONENTS "")
endmacro()

function(set_component_target COMPONENT target_name)
    set(${target_name} ${COMPONENT} PARENT_SCOPE)
    # set(${target_name} Component_${COMPONENT} PARENT_SCOPE)
endfunction()

# This is a private helper function which implements the recursion of the graph traversal
# algorithm.  The reason it's implemented using a macro instead of a function is because
# all variables set in functions are locally scoped, and the way to get around that, using
# set with PARENT_SCOPE, is shitty and does not behave in a predictable way in this setting.
# Also, the reason that the dumb nested if/else statements are used instead of early-out
# return statements is because returning from a macro actually returns from the function
# invoking it.
#
# This function CAN handle cyclic dependency graphs.
macro(_compute_all_component_dependencies_of COMPONENT RECURSION_INDENT PRINT_DEBUG_MESSAGES)
    set(_explicit_dependencies ${${COMPONENT}_EXPLICIT_COMPONENT_DEPENDENCIES})
    list(LENGTH _explicit_dependencies _explicit_dependency_count)

    # If COMPONENT has already been visited, return nothing
    list(FIND VISITED ${COMPONENT} _index)
    if(NOT ${_index} LESS 0) # If _index >= 0, then COMPONENT was found in _visited
        if(${PRINT_DEBUG_MESSAGES})
            message("${RECURSION_INDENT}visiting component ${COMPONENT}, visited [${VISITED}] ... base case -- already visited")
        endif()
    # If there are no explicit dependencies, return COMPONENT
    elseif(${_explicit_dependency_count} EQUAL 0)
        if(${PRINT_DEBUG_MESSAGES})
            message("${RECURSION_INDENT}visiting component ${COMPONENT}, visited [${VISITED}] ... base case -- no explicit dependencies")
        endif()
        list(APPEND VISITED ${COMPONENT}) # Mark COMPONENT as visited.
    # Otherwise there are unvisited dependencies to visit, so recurse on them.
    else()
        if(${PRINT_DEBUG_MESSAGES})
            message("${RECURSION_INDENT}visiting component ${COMPONENT}, visited [${VISITED}] ... recursing on dependencies [${_explicit_dependencies}]")
        endif()
        list(APPEND VISITED ${COMPONENT}) # Mark COMPONENT as visited.
        foreach(_dependency ${_explicit_dependencies})
            _compute_all_component_dependencies_of(${_dependency} "${RECURSION_INDENT}    " ${PRINT_DEBUG_MESSAGES})
        endforeach()
    endif()
endmacro()

# This function traverses the directed graph of component dependencies (there may be
# cycles of mutually-dependent components).  COMPONENT should be the component whose
# dependencies will be computed.  The output is placed in _retval_name, which will be
# set to the list of all dependencies of COMPONENT, and will be sorted alphabetically.
# COMPONENT is considered a dependency of itself.
#
# This function CAN handle cyclic dependency graphs.
function(compute_all_component_dependencies_of COMPONENT _retval_name)
    set(VISITED "")
    _compute_all_component_dependencies_of(${COMPONENT} "" 0)
    list(SORT VISITED)
    set(${_retval_name} ${VISITED} PARENT_SCOPE)
endfunction()

# TODO: use CMakePackageConfigHelpers: http://www.cmake.org/cmake/help/git-master/module/CMakePackageConfigHelpers.html#module:CMakePackageConfigHelpers
macro(define_component NAME SOURCES_TO_INSTALL SOURCES_NO_INSTALL RESOURCES_TO_INSTALL EXPLICIT_COMPONENT_DEPENDENCIES EXPLICIT_LIBRARY_DEPENDENCIES)
    # message("defining component \"${NAME}\" with:\n"
    #         "\tSOURCES_TO_INSTALL = ${SOURCES_TO_INSTALL}\n"
    #         "\tSOURCES_NO_INSTALL = ${SOURCES_NO_INSTALL}\n"
    #         "\tRESOURCES_TO_INSTALL = ${RESOURCES_TO_INSTALL}\n"
    #         "\tEXPLICIT_COMPONENT_DEPENDENCIES = ${EXPLICIT_COMPONENT_DEPENDENCIES}\n"
    #         "\tEXPLICIT_LIBRARY_DEPENDENCIES = ${EXPLICIT_LIBRARY_DEPENDENCIES}")
    set(DEFINED_COMPONENTS ${DEFINED_COMPONENTS} ${NAME})
    set(${NAME}_SOURCES ${SOURCES_TO_INSTALL} ${SOURCES_NO_INSTALL})
    set(${NAME}_SOURCES_TO_INSTALL ${SOURCES_TO_INSTALL})
    set(${NAME}_RESOURCES_TO_INSTALL ${RESOURCES_TO_INSTALL})
    set(${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES ${EXPLICIT_COMPONENT_DEPENDENCIES})
    set(${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES ${EXPLICIT_LIBRARY_DEPENDENCIES})
    # message("defined cmake variables:\n"
    #         "\t${NAME}_SOURCES = ${${NAME}_SOURCES}\n"
    #         "\t${NAME}_SOURCES_TO_INSTALL = ${${NAME}_SOURCES_TO_INSTALL}\n"
    #         "\t${NAME}_RESOURCES_TO_INSTALL = ${${NAME}_RESOURCES_TO_INSTALL}\n"
    #         "\t${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES = ${${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES}\n"
    #         "\t${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES = ${${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES}\n")
endmacro()

#include(TargetImportedLibraries)
include(CMakeParseArguments)
#include(PrintTargetProperties)

function(define_component_as_library COMPONENT)
    # Do the fancy map-style parsing of the arguments
    set(_options EXCLUDE_FROM_ALL)
    set(_one_value_args "") # None for now
    set(_multi_value_args
        HEADERS
        SOURCES
        COMPILE_DEFINITIONS
        # COMPILE_FEATURES # target_compile_features is not working for me.
        COMPILE_OPTIONS
        EXPLICIT_COMPONENT_DEPENDENCIES
        EXPLICIT_LIBRARY_DEPENDENCIES
        ADDITIONAL_TARGET_PROPERTIES
    )
    cmake_parse_arguments(_arg "${_options}" "${_one_value_args}" "${_multi_value_args}" ${ARGN})
  
    # Parse the arguments for use in the following target-defining calls.
    if(${_arg_EXCLUDE_FROM_ALL})
        set(_exclude_from_all "EXCLUDE_FROM_ALL")
    else()
        set(_exclude_from_all "")
    endif()

    # Determine the target name of this component.
    set_component_target(${COMPONENT} _component_target_name)
    # Determine the relative paths of all the headers.
    set(_headers "")
    foreach(_header ${_arg_HEADERS})
        list(APPEND _headers ${COMPONENT}/${_header})
    endforeach()
    # Determine the relative paths of all the sources.
    set(_sources "")
    foreach(_source ${_arg_SOURCES})
        list(APPEND _sources ${COMPONENT}/${_source})
    endforeach()

    # If there are no sources, add an empty dummy source file so that the linker has
    # something to chew on (there would be a linker-archiver error otherwise).  NOTE:
    # there will be a linker warning about an empty table of contents in the component's
    # library.
    list(LENGTH _sources _source_count)
    if(${_source_count} GREATER 0)
        add_library(${_component_target_name} ${_exclude_from_all} ${_headers} ${_sources})
    else()
        add_library(${_component_target_name} ${_exclude_from_all} ${_headers} empty.cpp)
    endif()

    # If this component has headers, then they must be located in a subdirectory with the same name.
    if(_arg_HEADERS)
        target_include_directories(
            ${_component_target_name}
            PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}>
                $<INSTALL_INTERFACE:${COMPONENT}>)
    endif()
    # If there are compile definitions, add them.
    if(_arg_COMPILE_DEFINITIONS)
        target_compile_definitions(${_component_target_name} PUBLIC ${_arg_COMPILE_DEFINITIONS})
    endif()
    # If there are compile options, add them.
    if(_arg_COMPILE_OPTIONS)
        target_compile_options(${_component_target_name} PUBLIC ${_arg_COMPILE_OPTIONS})
    endif()

    # Add link libraries from each component dependency.  The target_link_directories
    # command sets up the propagation of the various INTERFACE_XXX target properties
    # (e.g. INTERFACE_INCLUDE_DIRECTORIES, INTERFACE_COMPILE_OPTIONS,
    # INTERFACE_LINK_LIBRARIES) during build time from the dependencies to their
    # dependents.
    foreach(_dep ${_arg_EXPLICIT_COMPONENT_DEPENDENCIES})
        set_component_target(${_dep} _dep_target_name)
        # Specify the dependency.
        target_link_libraries(${_component_target_name} PUBLIC ${_dep_target_name})
    endforeach()

    # Add include directories and link libraries from each library dependency,
    # analogously to that of the component dependencies.  This requires calling
    # find_package on the libraries which haven't been loaded as targets yet.
    foreach(_dep ${_arg_EXPLICIT_LIBRARY_DEPENDENCIES})
        # For each library that hasn't been target_package'ed yet, call target_package on it.
        string(REPLACE " " ";" _semicolon_delimited_dep ${_dep})
        list(GET _semicolon_delimited_dep 0 _lib_name)
        set(_lib_target_name ${_lib_name}::${_lib_name})
        # message("checking if package ${_lib_target_name} or ${_lib_name} is already found")
        # if(NOT TARGET ${_lib_target_name} AND NOT TARGET ${_lib_name})
        if(NOT TARGET ${_lib_target_name})
            # message("it isn't -- calling find_package(${_semicolon_delimited_dep})")
            find_package(${_semicolon_delimited_dep})
            if(NOT TARGET ${_lib_target_name})
                message(ERROR " failed to find package ${_dep} -- expected target ${_lib_target_name}")
            endif()
        endif()
        # Specify the dependency.
        target_link_libraries(${_component_target_name} PUBLIC ${_lib_target_name})
    endforeach()

    # echo_target(${_component_target_name})

    # Add any other particular target properties (this overrides ones that have been set already).
    if(${_additional_target_property_count})
        set_target_properties(${_component_target_name} PROPERTIES ${_arg_ADDITIONAL_TARGET_PROPERTIES})
    endif()

    # Append this component to the list of defined components.
    set(DEFINED_COMPONENTS ${DEFINED_COMPONENTS} ${_component_target_name} PARENT_SCOPE)
endfunction()

function(define_install_rules TARGET DESIRED_COMPONENTS)
    # message("defining install rule for target ${TARGET}")
    set(INCLUDE_DIRECTORIES "")
    foreach(COMPONENT ${DESIRED_COMPONENTS}) # This loop should be replaced with determine_include_directories
        list(APPEND INCLUDE_DIRECTORIES include/${COMPONENT})
    endforeach()
    install(
        TARGETS ${TARGET}
        EXPORT ${TARGET}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION ${INCLUDE_DIRECTORIES})
    install(
        EXPORT ${TARGET}
        DESTINATION .
        FILE ComponentsConfig.cmake)
    # message("defining install rules for components ${DESIRED_COMPONENTS}")
    foreach(COMPONENT ${DESIRED_COMPONENTS})
        # message("defining install rules for component ${COMPONENT}")
        foreach(SOURCE_TO_INSTALL ${${COMPONENT}_SOURCES_TO_INSTALL})
            # message("\tdefining include file install rule ${COMPONENT}/${SOURCE_TO_INSTALL} -> include/${COMPONENT}/${SOURCE_TO_INSTALL}")
            install(
                FILES ${COMPONENT}/${SOURCE_TO_INSTALL}
                DESTINATION include/${COMPONENT}
            )
        endforeach()
        foreach(RESOURCE_TO_INSTALL ${${COMPONENT}_RESOURCES_TO_INSTALL})
            # message("\tdefining resource file install rule ${COMPONENT}/${RESOURCE_TO_INSTALL} -> resources/${RESOURCE_TO_INSTALL}")
            install(
                FILES ${COMPONENT}/${RESOURCE_TO_INSTALL}
                DESTINATION resources
            )
        endforeach()
    endforeach()
endfunction()

###################################################################################################
# Test functions
###################################################################################################

macro(check_deps COMPONENT_NAME EXPECTED_DEPS)
    compute_all_component_dependencies_of(${COMPONENT_NAME} DEPS)
    if("${DEPS}" STREQUAL "${EXPECTED_DEPS}")
        # message("dependencies of ${COMPONENT_NAME} = ${DEPS} -- got expected value")
    else()
        message(ERROR " dependencies of ${COMPONENT_NAME} = ${DEPS} -- expected ${EXPECTED_DEPS}")
    endif()
endmacro()

function(test_compute_all_component_dependencies_of)
    # Mutually-dependending components.
    define_component(A "" "" "" "B" "")
    define_component(B "" "" "" "A" "")
    check_deps(A "A;B")
    check_deps(B "A;B")

    # Self-dependending component (it's not necessary to specify self-dependency,
    # but it shouldn't hurt either).
    define_component(O "" "" "" "O" "")
    check_deps(O "O")

    # A 3-cycle of dependency.
    define_component(P "" "" "" "Q" "")
    define_component(Q "" "" "" "R" "")
    define_component(R "" "" "" "P" "")
    check_deps(P "P;Q;R")
    check_deps(Q "P;Q;R")
    check_deps(R "P;Q;R")

    # A diamond of dependency -- the more-northern components depend
    # on each more-southern components.
    define_component(N "" "" "" "W;E" "")
    define_component(W "" "" "" "S" "")
    define_component(E "" "" "" "S" "")
    define_component(S "" "" "" "" "")
    check_deps(N "E;N;S;W")
    check_deps(W "S;W")
    check_deps(E "E;S")
    check_deps(S "S")

    # TODO: make tests for other complicated graph cases?
endfunction()

