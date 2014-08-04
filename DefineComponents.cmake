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
# The source code in a component should generally be mutually dependent or otherwise
# inseparable.  Consider the set of all reasonable (non-contrived) applications that
# may use the code -- think of each source file to be a point in a plane.  Each
# application is going to use some subset of that source code, which corresponds to
# some subset of points in the plane.  For a given source file X, consider the set A
# of all applications that use it, and take the intersection I of the sets of source
# code that each application in A uses.  This will be some subset which contains X.
# The component that X should belong to should contain exactly the set of source code
# I, which may contain more than just X.  TODO: visual example
#
# The global variable DEFINED_COMPONENTS is a list containing the target names of each
# of the defined components, in the order they were defined.  The component name should
# be identical to the subdirectory which contains all its source files.  A component
# name must therefore be acceptable as a directory name and as a cmake target name.

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
endfunction()

include(CMakeParseArguments)

# This function defines a component (logical subgrouping of source, as described above)
# as a library target.  The function uses the CMakeParseArguments paradigm, where all-uppercase
# keywords indicate the meaning of the arguments that follow it (e.g. the install cmake command).
# The options are as follows:
#
# - Optional boolean arguments (an option's presence enables that argument, and its non-presence
#   implicitly disables that argument -- this is the default):
#   * EXCLUDE_FROM_ALL -- Excludes this component's target from the "make all" target.
# - Parameters taking a single argument:
#   * BRIEF_DOC_STRING <string> -- A brief description of this component which should fit within
#     one line (about 80 chars).
# - Parameters taking multiple arguments (each one is optional, unless otherwise specified):
#   * HEADERS [header1 [header2 [...]]] -- The list of headers for the component.  Each of these
#     should be specified using a relative path, based at the component's subdirectory.
#   * SOURCES [source1 [source2 [...]]] -- Analogous to HEADERS, but for source files.
#   * COMPILE_DEFINITIONS [def1 [def2 [...]]] -- Specifies which C preprocessor definitions to
#     pass to the compiler.  Each argument should be in the form
#       VAR
#     or
#       VAR=value
#     Currently, each compile definition is inherited by components which depend upon this component.
#   * COMPILE_OPTIONS [opt1 [opt2 [...]]] -- Specifies commandline flags to pass to the compiler.
#     Currently, each compile option is inherited by components which depend upon this component.
#   * EXPLICIT_COMPONENT_DEPENDENCIES [comp1 [comp2 [...]]] -- Specifies which other components
#     this component depends upon.  Each dependency must already be fully defined.  What this
#     dependency means on a practical level is that the compile definitions, compile options,
#     include directories, and link libraries will all be inherited by this component automatically.
#   * EXPLICIT_LIBRARY_DEPENDENCIES [lib1 [lib2 [...]]] -- Specifies the libraries that this
#     component depends upon.  Each parameter should be in the form
#       "LibName [version] [other-arguments]"
#     and will be passed verbatim as arguments to find_package (therefore see the documentation for
#     find_package for more details).  These libraries do not need to already be defined -- the
#     find_package function will be called on them (invoking the respective FindXXX.cmake module).
#     As with EXPLICIT_COMPONENT_DEPENDENCIES, the compile definitions, compile options, include
#     directories (and presumably link libraries?) will all be inherited by this component
#     automatically.
#   * ADDITIONAL_TARGET_PROPERTIES [prop1 val1 [prop2 val2 [...]]] -- Specifies arguments to pass
#     directly to set_target_properties, which will be called on the library target for this
#     component.  These target properties are set at the very end of this function, so any target
#     property already set can and will be overridden.  TODO: add a warning about overriding
#     properties?
#   * BRIEF_DOC_STRING [string1 [string2 [...]]] -- Specifies a list of strings (each of which
#     typically can be newline-terminated) whose concatenation defines the "brief description"
#     of this component.
#   * DETAILED_DOC_STRINGS [string1 [string2 [...]]] -- Analogous to BRIEF_DOC_STRING, but is
#     intended to be used for a more detailed description.  Each separate string should be
#     newline-terminated and should fit within one line (about 80 chars), so the print-out of all
#     the strings results is a reasonably-formatted block of text.
#
# The following target properties will automatically be set on the component's library target.
# Again, this is done before the ADDITIONAL_TARGET_PROPERTIES are set, so these can be overridden.
# - HEADERS                             -- As described above.
# - SOURCES                             -- As described above.
# - PATH_PREFIXED_HEADERS               -- The same as HEADERS, but with the component name as a path prefix.
# - PATH_PREFIXED_SOURCES               -- The same as SOURCES, but with the component name as a path prefix.
# - EXPLICIT_COMPONENT_DEPENDENCIES     -- As described above.
# - EXPLICIT_LIBRARY_DEPENDENCIES       -- As described above.
# - BRIEF_DOC_STRING                    -- As described above.
# - DETAILED_DOC_STRINGS                -- As described above.
# - IS_HEADER_ONLY                      -- Is set to TRUE if and only if there are no SOURCES,
#                                          and is otherwise set to FALSE.
# - IS_PHONY                            -- Is set to TRUE if and only if there are no HEADERS
#                                          and no SOURCES, i.e. if this is a "phony" target,
#                                          and is otherwise set to FALSE.
function(define_component_as_library COMPONENT)
    # Do the fancy map-style parsing of the arguments
    set(_options EXCLUDE_FROM_ALL)
    set(_one_value_args
        BRIEF_DOC_STRING        # A one-line, short (no more than about 80 chars) description of the component.
    )
    set(_multi_value_args
        HEADERS
        SOURCES
        COMPILE_DEFINITIONS
        # COMPILE_FEATURES      # target_compile_features is not working for me.
        COMPILE_OPTIONS
        EXPLICIT_COMPONENT_DEPENDENCIES
        EXPLICIT_LIBRARY_DEPENDENCIES
        ADDITIONAL_TARGET_PROPERTIES
        DETAILED_DOC_STRINGS    # This is for a more in-depth description of the purpose and scope of the component.
    )
    cmake_parse_arguments(_arg "${_options}" "${_one_value_args}" "${_multi_value_args}" ${ARGN})
  
    # Check the validity/presence of certain options
    if(NOT _arg_BRIEF_DOC_STRING)
        message(SEND_ERROR "Required BRIEF_DOC_STRING value was not defined for component ${COMPONENT}")
    endif()

    # Parse the arguments for use in the following target-defining calls.
    if(${_arg_EXCLUDE_FROM_ALL})
        set(_exclude_from_all "EXCLUDE_FROM_ALL")
    else()
        set(_exclude_from_all "")
    endif()

    # Determine the target name of this component.
    set_component_target(${COMPONENT} _component_target_name)
    # Determine the relative paths of all the headers.
    set(_path_prefixed_headers "")
    foreach(_header ${_arg_HEADERS})
        list(APPEND _path_prefixed_headers ${COMPONENT}/${_header})
    endforeach()
    # Determine the relative paths of all the sources.
    set(_path_prefixed_sources "")
    foreach(_source ${_arg_SOURCES})
        list(APPEND _path_prefixed_sources ${COMPONENT}/${_source})
    endforeach()

    # If there are no sources, add an empty dummy source file so that the linker has
    # something to chew on (there would be a linker-archiver error otherwise).  NOTE:
    # there will be a linker warning about an empty table of contents in the component's
    # library.
    # TODO: Consider using `add_library(target OBJECT ...)` to make a library target
    # which doesn't compile down to an archived library, but otherwise behaves as one.
    # This may have caveats, such as calling
    #   add_executable(user_app $<TARGET_OBJECTS:target>)
    # (essentially treating it as a set of sources) instead of
    #   target_link_libraries(user_app target)
    # See the docs for add_library.
    list(LENGTH _path_prefixed_sources _source_count)
    if(${_source_count} EQUAL 0)
        set(_is_header_only TRUE)
        add_library(${_component_target_name} ${_exclude_from_all} ${_path_prefixed_headers} empty.cpp)
    else()
        set(_is_header_only FALSE)
        add_library(${_component_target_name} ${_exclude_from_all} ${_path_prefixed_headers} ${_path_prefixed_sources})
    endif()

    # Determine if this is a "phony" target, meaning there are no headers or sources.
    list(LENGTH _path_prefixed_headers _header_count)
    if(${_header_count} EQUAL 0 AND ${_source_count} EQUAL 0)
        set(_is_phony TRUE)
    else()
        set(_is_phony FALSE)
    endif()

    # If this component has headers, then they must be located in a subdirectory with the same name.
    if(_arg_HEADERS)
        target_include_directories(
            ${_component_target_name}
            PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}>
                $<INSTALL_INTERFACE:include/${COMPONENT}>)
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
                message(SEND_ERROR "failed to find package ${_dep} -- expected target ${_lib_target_name}")
            endif()
        endif()
        # Specify the dependency.
        target_link_libraries(${_component_target_name} PUBLIC ${_lib_target_name})
    endforeach()

    # Store several of the parameter values as target properties
    set_target_properties(
        ${_component_target_name}
        PROPERTIES
            HEADERS "${_arg_HEADERS}"
            SOURCES "${_arg_SOURCES}"
            PATH_PREFIXED_HEADERS "${_path_prefixed_headers}"
            PATH_PREFIXED_SOURCES "${_path_prefixed_sources}"
            EXPLICIT_COMPONENT_DEPENDENCIES "${_arg_EXPLICIT_COMPONENT_DEPENDENCIES}"
            EXPLICIT_LIBRARY_DEPENDENCIES "${_arg_EXPLICIT_LIBRARY_DEPENDENCIES}"
            BRIEF_DOC_STRING "${_arg_BRIEF_DOC_STRING}"
            DETAILED_DOC_STRINGS "${_arg_DETAILED_DOC_STRINGS}"
            IS_HEADER_ONLY ${_is_header_only}
            IS_PHONY ${_is_phony}
    )

    # Add any other particular target properties.  NOTE: This should be done last, so it can override
    # any other property that has already been set.
    if(${_additional_target_property_count})
        set_target_properties(${_component_target_name} PROPERTIES ${_arg_ADDITIONAL_TARGET_PROPERTIES})
    endif()

    # Append this component to the list of defined components.
    set(DEFINED_COMPONENTS ${DEFINED_COMPONENTS} ${_component_target_name} PARENT_SCOPE)
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
    get_target_property(_explicit_dependencies ${COMPONENT} EXPLICIT_COMPONENT_DEPENDENCIES)
    # set(_explicit_dependencies ${${COMPONENT}_EXPLICIT_COMPONENT_DEPENDENCIES})
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

# This is a private helper function for print_dependency_graph_of_component.
function(_print_dependency_graph_of_component COMPONENT RECURSION_INDENT)
    get_target_property(_brief_doc_string ${COMPONENT} BRIEF_DOC_STRING)
    message("${RECURSION_INDENT}${COMPONENT} -- ${_brief_doc_string}")
    get_target_property(_explicit_component_dependencies ${COMPONENT} EXPLICIT_COMPONENT_DEPENDENCIES)
    foreach(_dep ${_explicit_component_dependencies})
        _print_dependency_graph_of_component(${_dep} "${RECURSION_INDENT}    ")
    endforeach()
endfunction()

# This function prints a dependency graph of the given component, simply using
# nested, indented text lines to denote dependency.
function(print_dependency_graph_of_component COMPONENT)
    _print_dependency_graph_of_component(${COMPONENT} "")
endfunction()

# This function prints a dependency graph for a library that explicitly depends on
# the components listed in LINK_COMPONENTS.
function(print_dependency_graph_of_component_linking_library LIBNAME LINK_COMPONENTS)
    message("${LIBNAME} -- depends explicitly on [${LINK_COMPONENTS}]")
    foreach(_link_component ${LINK_COMPONENTS})
        _print_dependency_graph_of_component(${_link_component} "    ")
    endforeach()
endfunction()

# TODO: write a dot graph generator which produces the dependency graph.

###################################################################################################
# Test functions
###################################################################################################

macro(check_deps COMPONENT_NAME EXPECTED_DEPS)
    compute_all_component_dependencies_of(${COMPONENT_NAME} DEPS)
    if("${DEPS}" STREQUAL "${EXPECTED_DEPS}")
        # message("dependencies of ${COMPONENT_NAME} = ${DEPS} -- got expected value")
    else()
        message(SEND_ERROR "dependencies of ${COMPONENT_NAME} = ${DEPS} -- expected ${EXPECTED_DEPS}")
    endif()
endmacro()

function(define_test_component COMPONENT EXPLICIT_COMPONENT_DEPENDENCIES)
    add_custom_target(${COMPONENT})
    set_target_properties(${COMPONENT} PROPERTIES EXPLICIT_COMPONENT_DEPENDENCIES "${EXPLICIT_COMPONENT_DEPENDENCIES}")
endfunction()

function(test_compute_all_component_dependencies_of)
    # Mutually-dependending components.
    define_test_component(A "B")
    define_test_component(B "A")
    check_deps(A "A;B")
    check_deps(B "A;B")

    # Self-dependending component (it's not necessary to specify self-dependency,
    # but it shouldn't hurt either).
    define_test_component(O "O")
    check_deps(O "O")

    # A 3-cycle of dependency.
    define_test_component(P "Q")
    define_test_component(Q "R")
    define_test_component(R "P")
    check_deps(P "P;Q;R")
    check_deps(Q "P;Q;R")
    check_deps(R "P;Q;R")

    # A diamond of dependency -- the more-northern components depend
    # on each more-southern components.
    define_test_component(N "W;E")
    define_test_component(W "S")
    define_test_component(E "S")
    define_test_component(S "")
    check_deps(N "E;N;S;W")
    check_deps(W "S;W")
    check_deps(E "E;S")
    check_deps(S "S")

    # TODO: make tests for other complicated graph cases?
endfunction()

# test_compute_all_component_dependencies_of()
