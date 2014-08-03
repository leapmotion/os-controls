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

# COMPONENTS is a list of strings enumerating all the defined components.  Defining a component
# should append to it -- the define_component macro is a convience macro for defining all the
# correct variables for a component.
macro(begin_component_definitions)
    set(COMPONENTS "")
endmacro()

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

include(TargetImportedLibraries)
include(CMakeParseArguments)
include(PrintTargetProperties)

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
        # INTERFACE_COMPONENT_DEPENDENCIES
        # PRIVATE_COMPONENT_DEPENDENCIES
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
    set(_component_target_name "Component_${COMPONENT}")
    print_value_of(_component_target_name "")
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

    # If there are no sources, it's necessary to specify the linker language.
    list(LENGTH _sources _source_count)
    if(${_source_count} GREATER 0)
        # message("calling add_library, creating target ${_component_target_name} with source files")
        add_library(${_component_target_name} ${_exclude_from_all} ${_headers} ${_sources})
    else()
        # message("calling add_library, creating target ${_component_target_name} WITHOUT source files")
        # NOTE: there will be a linker warning about an empty table of contents in the
        # component's library.
        add_library(${_component_target_name} ${_exclude_from_all} ${_headers} empty.cpp)

        # add_custom_target(${_component_target_name} ${_exclude_from_all} ${_headers}) # doesn't seem to work

        # add_library(${_component_target_name} ${_exclude_from_all} ${_headers})           # doesn't seem to work
        # set_target_properties(${_component_target_name} PROPERTIES LINKER_LANGUAGE CXX)
    endif()

    target_include_directories(${_component_target_name} PUBLIC ${COMPONENT})
    if(_arg_COMPILE_DEFINITIONS)
        target_compile_definitions(${_component_target_name} PUBLIC ${_arg_COMPILE_DEFINITIONS})
    endif()
    if(_arg_COMPILE_OPTIONS)
        target_compile_options(${_component_target_name} PUBLIC ${_arg_COMPILE_OPTIONS})
    endif()

    foreach(_dep ${_arg_EXPLICIT_COMPONENT_DEPENDENCIES})
        set(_dep_target_name "Component_${_dep}")
        message("    defining target rules for component dependency ${_dep_target_name}")

        get_target_property(_compile_definitions ${_dep_target_name} INTERFACE_COMPILE_DEFINITIONS)
        if(_compile_definitions)
            target_compile_definitions(${_component_target_name} PUBLIC ${_compile_definitions})
        endif()
        
        get_target_property(_compile_options ${_dep_target_name} INTERFACE_COMPILE_OPTIONS)
        if(_compile_options)
            target_compile_options(${_component_target_name} PUBLIC ${_compile_options})
        endif()
        
        get_target_property(_include_dirs ${_dep_target_name} INTERFACE_INCLUDE_DIRECTORIES)
        if(_include_dirs)
            target_include_directories(${_component_target_name} PUBLIC ${_include_dirs})
        endif()

        # print_value_of(_dep_target_name "    ")
        target_link_libraries(${_component_target_name} PUBLIC ${_dep_target_name})

        get_target_property(_link_libs ${_dep_target_name} INTERFACE_LINK_LIBRARIES)
        if(_link_libs)
            target_link_libraries(${_component_target_name} PUBLIC ${_link_libs})
        endif()
    endforeach()

    foreach(_dep ${_arg_EXPLICIT_LIBRARY_DEPENDENCIES})
        # For each library that hasn't been target_package'ed yet, call target_package on it.
        string(REPLACE " " ";" _semicolon_delimited_dep ${_dep})
        list(GET _semicolon_delimited_dep 0 _lib_name)
        set(_lib_target_name ${_lib_name}::${_lib_name})
        message("checking if package ${_lib_target_name} or ${_lib_name} is already found")
        if(NOT TARGET ${_lib_target_name} AND NOT TARGET ${_lib_name})
            message("it isn't -- calling find_package(${_semicolon_delimited_dep})")
            find_package(${_semicolon_delimited_dep})
        endif()
        
        if(TARGET ${_lib_target_name})
            # This is good
            message("package found -- target is ${_lib_target_name}")
        elseif(TARGET ${_lib_name})
            message("package found -- target is ${_lib_name}")
            set(_lib_target_name ${_lib_name})
        else()
            message(ERROR " failed to find package ${_dep}")
        endif()

        print_value_of(_lib_target_name "    ")
        target_link_libraries(${_component_target_name} PUBLIC ${_lib_target_name})
        # target_imported_libraries(${_component_target_name} ${_lib_target_name})

        # set(_dep_target_name "${_dep}::${_dep}")
        # if(NOT TARGET ${_dep_target_name})
        #     find_package(${_dep_target_name})
        # endif()

        get_target_property(_compile_definitions ${_lib_target_name} INTERFACE_COMPILE_DEFINITIONS)
        if(_compile_definitions)
            target_compile_definitions(${_component_target_name} PUBLIC ${_compile_definitions})
        endif()
        
        get_target_property(_compile_options ${_lib_target_name} INTERFACE_COMPILE_OPTIONS)
        if(_compile_options)
            target_compile_options(${_component_target_name} PUBLIC ${_compile_options})
        endif()
        
        get_target_property(_include_dirs ${_lib_target_name} INTERFACE_INCLUDE_DIRECTORIES)
        if(_include_dirs)
            target_include_directories(${_component_target_name} PUBLIC ${_include_dirs})
        endif()

        get_target_property(_link_libs ${_lib_target_name} INTERFACE_LINK_LIBRARIES)
        if(_link_libs)
            target_imported_libraries(${_component_target_name} PUBLIC ${_link_libs})
        endif()
    endforeach()

    # # The PRIVATE_COMPONENT_DEPENDENCIES should only be used for setting the PRIVATE
    # # values of target_include_directories and target_link_libraries on this component's target.
    # # This is used for when a dependency component is included ONLY in the implementation of this
    # # component, i.e. NOT in any of its headers, and therefore is used privately with respect
    # # to code that depends on this component.
    # foreach(_dep ${_arg_PRIVATE_COMPONENT_DEPENDENCIES})
    #     set(_dep_target_name "Component_${_dep}")
    #     get_target_property(_include_dirs ${_dep_target_name} INTERFACE_INCLUDE_DIRECTORIES)
    #     target_include_directories(${_component_target_name} PRIVATE ${_include_dirs})
    #     get_target_property(_link_libs ${_dep_target_name} INTERFACE_LINK_LIBRARIES)
    #     target_link_libraries(${_component_target_name} PRIVATE ${_link_libs})
    # endforeach()

    # # The INTERFACE_COMPONENT_DEPENDENCIES should only be used for setting the INTERFACE
    # # values of target_include_directories and target_link_libraries on this component's target.
    # # This is used for when a dependency component is included in some header of this component,
    # # and therefore is used publicly with respect to code that depends on this component.
    # foreach(_dep ${_arg_INTERFACE_COMPONENT_DEPENDENCIES})
    #     set(_dep_target_name "Component_${_dep}")
    #     get_target_property(_include_dirs ${_dep_target_name} INTERFACE_INCLUDE_DIRECTORIES)
    #     target_include_directories(${_component_target_name} INTERFACE ${_include_dirs})
    #     get_target_property(_link_libs ${_dep_target_name} INTERFACE_LINK_LIBRARIES)
    #     target_link_libraries(${_component_target_name} INTERFACE ${_link_libs})
    # endforeach()

    # # It is necessary at this point to compute all the component dependencies of this
    # # component, via the directed graph of component dependencies.  NOTE: For this
    # # particular implementation (where the target_include_directories, etc functions
    # # are being called here, it is necessary for the component dependencies to already
    # # be declared, meaning that the dependency graph must be acyclic -- a DAG).
    # compute_all_component_dependencies_of(${COMPONENT} _all_component_dependencies)
    # foreach(_component_dependency ${_component_dependencies})
    #     # Retrieve the INTERFACE_INCLUDE_DIRECTORIES property from each component dependency 
    #     # target and specify it as an 
    #     target_include_directories(${_component_target_name} )
    # endforeach()


    # # TODO: figure out if these should be PUBLIC, PRIVATE, INTERFACE, etc.
    # # target_include_directories(${_component_target_name} PUBLIC ${_component_target_name}) # This will be done later
    # target_compile_definitions(${_component_target_name} PUBLIC ${_arg_COMPILE_DEFINITIONS})
    # # target_compile_features(${_component_target_name} PUBLIC ${_arg_COMPILE_FEATURES})
    # target_compile_options(${_component_target_name} PUBLIC ${_arg_COMPILE_OPTIONS})
    # # TODO: call target_sources instead of adding sources via add_library?

    # # Some of these might not be necessary
    # set_target_properties(
    #     ${_component_target_name}
    #     PROPERTIES
    #         EXPLICIT_COMPONENT_DEPENDENCIES "${_arg_EXPLICIT_COMPONENT_DEPENDENCIES}"
    #         EXPLICIT_LIBRARY_DEPENDENCIES "${_arg_EXPLICIT_LIBRARY_DEPENDENCIES}"
    # )
    # foreach(_explicit_component_dependency ${_arg_EXPLICIT_COMPONENT_DEPENDENCIES})
    #     # TODO: which one is appropriate?  PUBLIC PRIVATE INTERFACE LINK_INTERFACE_LIBRARIES
    #     # or should the caller of this function specify that parameter?
    #     set(_explicit_component_dependency_target_name "Component_${_explicit_component_dependency}")
    #     target_link_libraries(${_component_target_name} PUBLIC ${_explicit_component_dependency_target_name})
    # endforeach()

    # echo_target(${_component_target_name})

    # get_target_property(_incl_dirs ${_component_target_name} INCLUDE_DIRECTORIES)
    # message("INCLUDE_DIRECTORIES for target ${_component_target_name}:")
    # foreach(_incl_dir ${_incl_dirs})
    #     message("    ${_incl_dir}")
    # endforeach()

    # get_target_property(_incl_dirs ${_component_target_name} INTERFACE_INCLUDE_DIRECTORIES)
    # message("INTERFACE_INCLUDE_DIRECTORIES for target ${_component_target_name}:")
    # foreach(_incl_dir ${_incl_dirs})
    #     message("    ${_incl_dir}")
    # endforeach()

    # foreach(_explicit_library_dependency ${_arg_EXPLICIT_LIBRARY_DEPENDENCIES})
    #     # For each library that hasn't been target_package'ed yet, call target_package on it.
    #     string(REPLACE " " ";" _semicolon_delimited_library_dependency ${_explicit_library_dependency})
    #     list(GET _semicolon_delimited_library_dependency 0 _lib_name)
    #     set(_lib_target_name ${_lib_name}::${_lib_name})
    #     if(NOT TARGET ${_lib_target_name})
    #         find_package(${_semicolon_delimited_library_dependency})
    #     endif()
    #     if(NOT TARGET ${_lib_target_name})
    #         message(ERROR " failed to find package ${_lib_name} (expected target ${_lib_target_name})")
    #     endif()
    #     target_link_libraries(${_component_target_name} PUBLIC ${_lib_target_name})
    # endforeach()

    # Add any other particular target properties (this overrides ones that have been set already).
    if(${_additional_target_property_count})
        set_target_properties(${_component_target_name} PROPERTIES ${_arg_ADDITIONAL_TARGET_PROPERTIES})
    endif()

    message("\n")
endfunction()


# TODO: use http://www.cmake.org/cmake/help/git-master/module/CMakeParseArguments.html
# TODO: add compiler options and compiler definitions?
function(add_library_for_defined_component NAME)
    set(_component_target "Component_${NAME}")
    # Determine the sources (with the component subdir prepended)
    set(_sources "")
    foreach(_source ${${NAME}_SOURCES})
        list(APPEND _sources "${NAME}/${_source}")
    endforeach()
    message("add_library_for_defined_component NAME = ${NAME}, _component_target = ${_component_target}, _sources = ${_sources}")
    # Create the component target library
    # TODO: figure out how to cope with components that are header-only and would
    # have nothing in the library file.
    add_library(${_component_target} ${_sources})
    set_target_properties(${_component_target} PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${_component_target} PUBLIC ${NAME})

    message("${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES = ${${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES}")
    set(_explicit_component_dependencies ${${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES})
    foreach(_explicit_component_dependency ${_explicit_component_dependencies})
        message("    calling target_link_libraries(${_component_target} PUBLIC ${_explicit_component_dependency}")
        target_link_libraries(${_component_target} PUBLIC ${_explicit_component_dependency})
    endforeach()

    message("${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES = ${${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES}")
    # set(_explicit_library_dependencies ${${NAME}_EXPLICIT_LIBRARY_DEPENDENCIES})
    # foreach(_explicit_library_dependency ${_explicit_library_dependencies})
    #     set(_lib_target_name ${_explicit_library_dependency}::${_explicit_library_dependency})
    #     message("    calling target_link_libraries(${_component_target} PUBLIC ${_lib_target_name}")
    #     target_link_libraries(${_component_target} PUBLIC ${_lib_target_name})
    # endforeach()

    # Normally target_link_libraries would be called here on all the component dependencies,
    # but we have to wait until they're all defined, because there may be mutually-depending
    # components.


    # # Now ${_component_target} is a target, and we will set its target properties to
    # # configure its build rules.  Define the include directories for _component_target
    # # via all of its dependencies.  Note that a component is a dependency of itself,
    # # so its own include directories will also be added here.
    # compute_all_component_dependencies_of(${NAME} _component_dependencies)
    # message("    component dependencies: [${_component_dependencies}]")
    # # set(_include_directories "")
    # foreach(_dependency ${_component_dependencies})
    #     target_include_directories(${_component_target} PRIVATE ${_dependency})
    #     target_include_directories(${_component_target} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_dependency}>)
    # endforeach()
    # # Set all the link libraries, which must be compiled from the library dependencies
    # # of all the dependencies of this component.  The result will be a sorted list of
    # # unique library dependencies.  NOTE: This won't correctly handle non-exact version
    # # or case matches, so the following three library dependencies are all mutually
    # # distinct: "FreeImage 3.1", "freeimage 3.1" "FreeImage 3"
    # set(_library_dependencies "")
    # foreach(_dependency ${_component_dependencies})
    #     list(APPEND _library_dependencies ${${_dependency}_EXPLICIT_LIBRARY_DEPENDENCIES})
    # endforeach()
    # list(SORT _library_dependencies)
    # list(REMOVE_DUPLICATES _library_dependencies)
    # message("    library dependencies: [${_library_dependencies}]")
    # # Now call target_package on each of the library dependencies.
    # foreach(_library_dependency ${_library_dependencies})
    #     # A library dependency will have the form "LibName" or "LibName VersionNum",
    #     # and to correctly pass this into target_package, spaces must be converted
    #     # into semicolons.
    #     string(REPLACE " " ";" _semicolon_delimited_library_dependency ${_library_dependency})
    #     message("        semicolon-delimited: ${_semicolon_delimited_library_dependency}")
    #     # target_package(${_component_target} ${_semicolon_delimited_library_dependency} REQUIRED)
    #     # target_package(${_component_target} ${_library_dependency} REQUIRED)
    # endforeach()

    message("\n")
endfunction()

function(add_library_for_each_defined_component DEFINED_COMPONENTS)
    message("calling add_library_for_each_defined_component on ${DEFINED_COMPONENTS}")
    foreach(_defined_component ${DEFINED_COMPONENTS})
        add_library_for_defined_component(${_defined_component})
    endforeach()
endfunction()

# function(call_target_package_for_required_library LIBRARY) # Allow extra args, e.g. version
#     target_package()
# endfunction()

# function(link_libraries_for_defined_component NAME)
#     set(_component_target "Components_${NAME}")
#     set(_explicit_component_dependencies ${${NAME}_EXPLICIT_COMPONENT_DEPENDENCIES})
#     foreach(_explicit_component_dependency ${_explicit_component_dependencies})
#         target_link_libraries(${_component_target} PUBLIC ${_explicit_component_dependency})
#     endforeach()
# endfunction()

# function()

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

