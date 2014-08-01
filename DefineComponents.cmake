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

# COMPONENTS is a list of strings enumerating all the defined components.  Defining a component
# should append to it -- the define_component macro is a convience macro for defining all the
# correct variables for a component.
macro(begin_component_definitions)
    set(COMPONENTS "")
endmacro()

# include(TargetImportedLibraries)

# This is a private helper function which implements the recursion of the graph traversal
# algorithm.  The reason it's implemented using a macro instead of a function is because
# all variables set in functions are locally scoped, and the way to get around that, using
# set with PARENT_SCOPE, is shitty and does not behave in a predictable way in this setting.
# Also, the reason that the dumb nested if/else statements are used instead of early-out
# return statements is because returning from a macro actually returns from the function
# invoking it.
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
# set to the list of all dependencies of COMPONENT.  COMPONENT is considered a dependency
# of itself.
function(compute_all_component_dependencies_of COMPONENT _retval_name)
    set(VISITED "")
    _compute_all_component_dependencies_of(${COMPONENT} "" 0)
    # TODO: maybe sort the elements of VISITED
    set(${_retval_name} ${VISITED} PARENT_SCOPE)
endfunction()

function(define_component_as_library NAME SOURCES_TO_INSTALL SOURCES_NO_INSTALL RESOURCES_TO_INSTALL EXPLICIT_COMPONENT_DEPENDENCIES EXPLICIT_LIBRARY_DEPENDENCIES)
    set(SOURCES ${SOURCES_TO_INSTALL} ${SOURCES_NO_INSTALL})
    set(COMPONENT_TARGET "Components_${NAME}")
    message("define_component_as_library NAME = ${NAME}, COMPONENT_TARGET = ${COMPONENT_TARGET}")
    add_library(${COMPONENT_TARGET} ${SOURCES})
    # Now ${COMPONENT_TARGET} is a target, and we will set its target properties to
    # configure its build rules.
    target_include_directories(${COMPONENT_TARGET} PRIVATE ${NAME})
    target_include_directories(${COMPONENT_TARGET} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${NAME}>)
    # Define the include directories for COMPONENT_TARGET recursively via its explicit dependencies
    message("recursively obtaining include directories from explicit component dependencies: ${EXPLICIT_COMPONENT_DEPENDENCIES}")
    foreach(EXPLICIT_COMPONENT_DEPENDENCY ${EXPLICIT_COMPONENT_DEPENDENCIES})
        # TODO: check that ${EXPLICIT_COMPONENT_DEPENDENCY} is a defined target
        set(EXPLICIT_COMPONENT_DEPENDENCY_TARGET "Components_${EXPLICIT_COMPONENT_DEPENDENCY}")
        # Use EXPLICIT_COMPONENT_DEPENDENCY_TARGET's include directories
        get_target_property(
            EXPLICIT_COMPONENT_DEPENDENCY_INCLUDE_DIRECTORIES
            ${EXPLICIT_COMPONENT_DEPENDENCY_TARGET}
            INCLUDE_DIRECTORIES)
        get_target_property(
            EXPLICIT_COMPONENT_DEPENDENCY_INTERFACE_INCLUDE_DIRECTORIES
            ${EXPLICIT_COMPONENT_DEPENDENCY_TARGET}
            INTERFACE_INCLUDE_DIRECTORIES)
        message(
            "    EXPLICIT_COMPONENT_DEPENDENCY_TARGET = ${EXPLICIT_COMPONENT_DEPENDENCY_TARGET},\n"
            "    INCLUDE_DIRECTORIES = ${EXPLICIT_COMPONENT_DEPENDENCY_INCLUDE_DIRECTORIES},\n"
            "    INTERFACE_INCLUDE_DIRECTORIES = ${EXPLICIT_COMPONENT_DEPENDENCY_INTERFACE_INCLUDE_DIRECTORIES}\n")
        # target_include_directories(${COMPONENT_TARGET} PRIVATE ${EXPLICIT_COMPONENT_DEPENDENCY_INCLUDE_DIRECTORIES})
        # target_include_directories(${COMPONENT_TARGET} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${EXPLICIT_COMPONENT_DEPENDENCY_TARGET}>)
    endforeach()
endfunction()

# TODO: use CMakePackageConfigHelpers: http://www.cmake.org/cmake/help/git-master/module/CMakePackageConfigHelpers.html#module:CMakePackageConfigHelpers
macro(define_component NAME SOURCES_TO_INSTALL SOURCES_NO_INSTALL RESOURCES_TO_INSTALL EXPLICIT_COMPONENT_DEPENDENCIES EXPLICIT_LIBRARY_DEPENDENCIES)
    # message("defining component \"${NAME}\" with:\n"
    #         "\tSOURCES_TO_INSTALL = ${SOURCES_TO_INSTALL}\n"
    #         "\tSOURCES_NO_INSTALL = ${SOURCES_NO_INSTALL}\n"
    #         "\tRESOURCES_TO_INSTALL = ${RESOURCES_TO_INSTALL}\n"
    #         "\tEXPLICIT_COMPONENT_DEPENDENCIES = ${EXPLICIT_COMPONENT_DEPENDENCIES}\n"
    #         "\tEXPLICIT_LIBRARY_DEPENDENCIES = ${EXPLICIT_LIBRARY_DEPENDENCIES}")
    set(COMPONENTS ${COMPONENTS} ${NAME})
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

# # TODO MAKE THIS FUNCTION WORK
# function(determine_include_directories DESIRED_COMPONENTS INCLUDE_DIRECTORIES)
#     set(${INCLUDE_DIRECTORIES} "")
#     foreach(COMPONENT ${DESIRED_COMPONENTS})
#         #list(APPEND ${INCLUDE_DIRECTORIES} include/${COMPONENT})
#         set(${INCLUDE_DIRECTORIES} ${${INCLUDE_DIRECTORIES}} include/${COMPONENT})
#     endforeach()
#     message("hippo INCLUDE_DIRECTORIES = ${INCLUDE_DIRECTORIES}")
# endfunction()

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
