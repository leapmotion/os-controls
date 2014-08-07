#.rst
# AddSublibrary
# ----------------
#
# Created by Victor Dods
# Functions and macros which assist in defining "sublibraries".
#
# A "sublibrary" can be thought of as a "sub-library" (in the sense that it is a small library and
# is subordinate to the whole library).  A sublibrary satisfies two requirements:
# - Has a well-defined purpose, scope, and feature set.
# - Has well-defined dependencies, which are explicitly declared and are minimal.
#
# What is a good criteria for deciding how to group source code into sublibraries?
# The source code in a sublibrary should generally be mutually dependent or otherwise
# inseparable.  Consider the set of all reasonable (non-contrived) applications that
# may use the code -- think of each source file to be a point in a plane.  Each
# application is going to use some subset of that source code, which corresponds to
# some subset of points in the plane.  For a given source file X, consider the set A
# of all applications that use it, and take the intersection I of the sets of source
# code that each application in A uses.  This will be some subset which contains X.
# The sublibrary that X should belong to should contain exactly the set of source code
# I, which may contain more than just X.  TODO: visual example
#
# The global variable ADDED_SUBLIBRARIES is a list containing the target names of each
# of the defined sublibraries, in the order they were defined.  The sublibrary name should
# be identical to the subdirectory which contains all its source files.  A sublibrary
# name must therefore be acceptable as both a directory name and as a cmake target name.

include(CMakeParseArguments)
include(VerboseMessage)

# ADDED_SUBLIBRARIES is a list of the target names of all the defined sublibraries.  Defining a
# sublibrary via the add_sublibrary will append to it.
macro(begin_sublibrary_definitions)
    set(ADDED_SUBLIBRARIES "")
    set(LIBRARY_DEPENDENCY_MAP "")
endmacro()

function(get_sublibrary_target_name SUBLIBRARY target_name)
    set(${target_name} ${SUBLIBRARY} PARENT_SCOPE)
endfunction()

# This function defines a sublibrary (logical subgrouping of source, as described above)
# as a library target.  The function uses the CMakeParseArguments paradigm, where all-uppercase
# keywords indicate the meaning of the arguments that follow it (e.g. the install cmake command).
# The options are as follows:
#
# - Optional boolean arguments (an option's presence enables that argument, and its non-presence
#   implicitly disables that argument -- this is the default):
#   * EXCLUDE_FROM_ALL -- Excludes this sublibrary's target from the "make all" target.
# - Parameters taking a single argument:
#   * BRIEF_DOC_STRING <string> -- A brief description of this sublibrary which should fit within
#     one line (about 80 chars).
# - Parameters taking multiple arguments (each one is optional, unless otherwise specified):
#   * HEADERS [header1 [header2 [...]]] -- The list of headers for the sublibrary.  Each of these
#     should be specified using a relative path, based at the sublibrary's subdirectory.
#   * SOURCES [source1 [source2 [...]]] -- Analogous to HEADERS, but for source files.
#   * COMPILE_DEFINITIONS [def1 [def2 [...]]] -- Specifies which C preprocessor definitions to
#     pass to the compiler.  Each argument should be in the form
#       VAR
#     or
#       VAR=value
#     Currently, each compile definition is inherited by sublibraries which depend upon this sublibrary.
#   * COMPILE_OPTIONS [opt1 [opt2 [...]]] -- Specifies commandline flags to pass to the compiler.
#     Currently, each compile option is inherited by sublibraries which depend upon this sublibrary.
#   * EXPLICIT_SUBLIBRARY_DEPENDENCIES [comp1 [comp2 [...]]] -- Specifies which other sublibraries
#     this sublibrary depends upon.  Each dependency must already be fully defined.  What this
#     dependency means on a practical level is that the compile definitions, compile options,
#     include directories, and link libraries will all be inherited by this sublibrary automatically.
#   * EXPLICIT_LIBRARY_DEPENDENCIES [lib1 [lib2 [...]]] -- Specifies the libraries that this
#     sublibrary depends upon.  Each parameter should be in the form
#       "LibName [version] [other-arguments]"
#     and will be passed verbatim as arguments to find_package (therefore see the documentation for
#     find_package for more details).  These libraries do not need to already be defined -- the
#     find_package function will be called on them (invoking the respective FindXXX.cmake module).
#     As with EXPLICIT_SUBLIBRARY_DEPENDENCIES, the compile definitions, compile options, include
#     directories (and presumably link libraries?) will all be inherited by this sublibrary
#     automatically.
#   * ADDITIONAL_TARGET_PROPERTIES [prop1 val1 [prop2 val2 [...]]] -- Specifies arguments to pass
#     directly to set_target_properties, which will be called on the library target for this
#     sublibrary.  These target properties are set at the very end of this function, so any target
#     property already set can and will be overridden.  TODO: add a warning about overriding
#     properties?
#   * BRIEF_DOC_STRING [string1 [string2 [...]]] -- Specifies a list of strings (each of which
#     typically can be newline-terminated) whose concatenation defines the "brief description"
#     of this sublibrary.
#   * DETAILED_DOC_STRINGS [string1 [string2 [...]]] -- Analogous to BRIEF_DOC_STRING, but is
#     intended to be used for a more detailed description.  Each separate string should be
#     newline-terminated and should fit within one line (about 80 chars), so the print-out of all
#     the strings results is a reasonably-formatted block of text.
#
# The following target properties will automatically be set on the sublibrary's library target.
# Again, this is done before the ADDITIONAL_TARGET_PROPERTIES are set, so these can be overridden.
# - HEADERS                             -- As described above.
# - SOURCES                             -- As described above.
# - PATH_PREFIXED_HEADERS               -- The same as HEADERS, but with the sublibrary name as a path prefix.
# - PATH_PREFIXED_SOURCES               -- The same as SOURCES, but with the sublibrary name as a path prefix.
# - EXPLICIT_SUBLIBRARY_DEPENDENCIES     -- As described above.
# - EXPLICIT_LIBRARY_DEPENDENCIES       -- As described above.
# - BRIEF_DOC_STRING                    -- As described above.
# - DETAILED_DOC_STRINGS                -- As described above.
# - IS_HEADER_ONLY                      -- Is set to TRUE if and only if there are no SOURCES,
#                                          and is otherwise set to FALSE.
# - IS_PHONY                            -- Is set to TRUE if and only if there are no HEADERS
#                                          and no SOURCES, i.e. if this is a "phony" target,
#                                          and is otherwise set to FALSE.
function(add_sublibrary SUBLIBRARY_NAME)
    verbose_message("add_sublibrary(${SUBLIBRARY_NAME} ...)")

    # Do the fancy map-style parsing of the arguments
    set(_options EXCLUDE_FROM_ALL)
    set(_one_value_args
        BRIEF_DOC_STRING        # A one-line, short (no more than about 80 chars) description of the sublibrary.
    )
    set(_multi_value_args
        HEADERS
        SOURCES
        COMPILE_DEFINITIONS
        # COMPILE_FEATURES      # target_compile_features is not working for me.
        COMPILE_OPTIONS
        EXPLICIT_SUBLIBRARY_DEPENDENCIES
        EXPLICIT_LIBRARY_DEPENDENCIES
        ADDITIONAL_TARGET_PROPERTIES
        DETAILED_DOC_STRINGS    # This is for a more in-depth description of the purpose and scope of the sublibrary.
    )
    cmake_parse_arguments(_arg "${_options}" "${_one_value_args}" "${_multi_value_args}" ${ARGN})
  
    # Check the validity/presence of certain options
    if(NOT _arg_BRIEF_DOC_STRING)
        message(SEND_ERROR "Required BRIEF_DOC_STRING value was not defined for sublibrary ${SUBLIBRARY_NAME}")
    endif()

    # Parse the arguments for use in the following target-defining calls.
    if(${_arg_EXCLUDE_FROM_ALL})
        set(_exclude_from_all "EXCLUDE_FROM_ALL")
    else()
        set(_exclude_from_all "")
    endif()

    # Determine the target name of this sublibrary.
    get_sublibrary_target_name(${SUBLIBRARY_NAME} _sublibrary_target_name)
    # Determine the relative paths of all the headers.
    set(_path_prefixed_headers "")
    foreach(_header ${_arg_HEADERS})
        list(APPEND _path_prefixed_headers ${SUBLIBRARY_NAME}/${_header})
    endforeach()
    # Determine the relative paths of all the sources.
    set(_path_prefixed_sources "")
    foreach(_source ${_arg_SOURCES})
        list(APPEND _path_prefixed_sources ${SUBLIBRARY_NAME}/${_source})
    endforeach()

    # If there are no sources, add an empty dummy source file so that the linker has
    # something to chew on (there would be a linker-archiver error otherwise).  NOTE:
    # there will be a linker warning about an empty table of contents in the sublibrary's
    # library file (the file with extension .lib or .a depending on the platform).
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
        add_library(${_sublibrary_target_name} ${_exclude_from_all} ${_path_prefixed_headers} empty.cpp)
    else()
        set(_is_header_only FALSE)
        add_library(${_sublibrary_target_name} ${_exclude_from_all} ${_path_prefixed_headers} ${_path_prefixed_sources})
    endif()

    # Determine if this is a "phony" target, meaning there are no headers or sources.
    list(LENGTH _path_prefixed_headers _header_count)
    if(${_header_count} EQUAL 0 AND ${_source_count} EQUAL 0)
        set(_is_phony TRUE)
    else()
        set(_is_phony FALSE)
    endif()

    # If this sublibrary has headers, then they must be located in a subdirectory with the same name.
    if(_arg_HEADERS)
        target_include_directories(
            ${_sublibrary_target_name}
            PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SUBLIBRARY_NAME}>
                $<INSTALL_INTERFACE:include/${SUBLIBRARY_NAME}>)
    endif()
    # If there are compile definitions, add them.
    if(_arg_COMPILE_DEFINITIONS)
        target_compile_definitions(${_sublibrary_target_name} PUBLIC ${_arg_COMPILE_DEFINITIONS})
    endif()
    # If there are compile options, add them.
    if(_arg_COMPILE_OPTIONS)
        target_compile_options(${_sublibrary_target_name} PUBLIC ${_arg_COMPILE_OPTIONS})
    endif()

    # Add link libraries from each sublibrary dependency.  The target_link_directories
    # command sets up the propagation of the various INTERFACE_XXX target properties
    # (e.g. INTERFACE_INCLUDE_DIRECTORIES, INTERFACE_COMPILE_OPTIONS,
    # INTERFACE_LINK_LIBRARIES) during build time from the dependencies to their
    # dependents.
    foreach(_dep ${_arg_EXPLICIT_SUBLIBRARY_DEPENDENCIES})
        get_sublibrary_target_name(${_dep} _dep_target_name)
        if(NOT TARGET ${_dep_target_name})
            message(SEND_ERROR "sublibrary \"${_dep}\" can't be depended upon -- it hasn't been defined yet!")
        endif()
        # Specify the dependency.
        target_link_libraries(${_sublibrary_target_name} PUBLIC ${_dep_target_name})
    endforeach()

    # Add include directories and link libraries from each library dependency,
    # analogously to that of the sublibrary dependencies.  This requires calling
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
        target_link_libraries(${_sublibrary_target_name} PUBLIC ${_lib_target_name})
    endforeach()

    # Store several of the parameter values as target properties
    set_target_properties(
        ${_sublibrary_target_name}
        PROPERTIES
            HEADERS "${_arg_HEADERS}"
            SOURCES "${_arg_SOURCES}"
            PATH_PREFIXED_HEADERS "${_path_prefixed_headers}"
            PATH_PREFIXED_SOURCES "${_path_prefixed_sources}"
            EXPLICIT_SUBLIBRARY_DEPENDENCIES "${_arg_EXPLICIT_SUBLIBRARY_DEPENDENCIES}"
            EXPLICIT_LIBRARY_DEPENDENCIES "${_arg_EXPLICIT_LIBRARY_DEPENDENCIES}"
            BRIEF_DOC_STRING "${_arg_BRIEF_DOC_STRING}"
            DETAILED_DOC_STRINGS "${_arg_DETAILED_DOC_STRINGS}"
            IS_HEADER_ONLY ${_is_header_only}
            IS_PHONY ${_is_phony}
    )

    # Add any other particular target properties.  NOTE: This should be done last, so it can override
    # any other property that has already been set.
    if(${_additional_target_property_count})
        set_target_properties(${_sublibrary_target_name} PROPERTIES ${_arg_ADDITIONAL_TARGET_PROPERTIES})
    endif()

    # Append this sublibrary to the list of defined sublibraries.
    set(ADDED_SUBLIBRARIES ${ADDED_SUBLIBRARIES} ${_sublibrary_target_name} PARENT_SCOPE)
    # For later generation of automatic library dependency finding, determine all library dependencies
    # of the added sublibrary recursively.
    compute_all_sublibrary_dependencies_of(${_sublibrary_target_name} _deps)
    set(_all_library_dependencies "")
    foreach(_dep ${_deps})
        get_target_property(_dep_explicit_library_dependencies ${_dep} EXPLICIT_LIBRARY_DEPENDENCIES)
        list(APPEND _all_library_dependencies ${_dep_explicit_library_dependencies})
    endforeach()
    list(SORT _all_library_dependencies)
    list(REMOVE_DUPLICATES _all_library_dependencies)
    verbose_message("all library dependencies of ${_sublibrary_target_name} : ${_all_library_dependencies}")
    # Store the dependencies in a "map" format which can be later parsed by cmake_parse_arguments.
    set(LIBRARY_DEPENDENCY_MAP ${LIBRARY_DEPENDENCY_MAP} ${_sublibrary_target_name} ${_all_library_dependencies} PARENT_SCOPE)
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
macro(_compute_all_sublibrary_dependencies_of SUBLIBRARY RECURSION_INDENT PRINT_DEBUG_MESSAGES)
    get_target_property(_explicit_dependencies ${SUBLIBRARY} EXPLICIT_SUBLIBRARY_DEPENDENCIES)
    list(LENGTH _explicit_dependencies _explicit_dependency_count)

    # If SUBLIBRARY has already been visited, return nothing
    list(FIND VISITED ${SUBLIBRARY} _index)
    if(NOT ${_index} LESS 0) # If _index >= 0, then SUBLIBRARY was found in _visited
        if(${PRINT_DEBUG_MESSAGES})
            message("${RECURSION_INDENT}visiting sublibrary ${SUBLIBRARY}, visited [${VISITED}] ... base case -- already visited")
        endif()
    # If there are no explicit dependencies, return SUBLIBRARY
    elseif(${_explicit_dependency_count} EQUAL 0)
        if(${PRINT_DEBUG_MESSAGES})
            message("${RECURSION_INDENT}visiting sublibrary ${SUBLIBRARY}, visited [${VISITED}] ... base case -- no explicit dependencies")
        endif()
        list(APPEND VISITED ${SUBLIBRARY}) # Mark SUBLIBRARY as visited.
    # Otherwise there are unvisited dependencies to visit, so recurse on them.
    else()
        if(${PRINT_DEBUG_MESSAGES})
            message("${RECURSION_INDENT}visiting sublibrary ${SUBLIBRARY}, visited [${VISITED}] ... recursing on dependencies [${_explicit_dependencies}]")
        endif()
        list(APPEND VISITED ${SUBLIBRARY}) # Mark SUBLIBRARY as visited.
        foreach(_dependency ${_explicit_dependencies})
            _compute_all_sublibrary_dependencies_of(${_dependency} "${RECURSION_INDENT}    " ${PRINT_DEBUG_MESSAGES})
        endforeach()
    endif()
endmacro()

# This function traverses the directed graph of sublibrary dependencies (there may be
# cycles of mutually-dependent sublibraries, though add_sublibrary is incapable
# of creating such cycles).  SUBLIBRARY should be the sublibrary whose dependencies will be 
# computed.  The output is placed in _retval_name, which will be set to the list of all
# dependencies of SUBLIBRARY, and will be sorted alphabetically.  SUBLIBRARY is considered
# a dependency of itself.
#
# This function CAN handle cyclic dependency graphs.
function(compute_all_sublibrary_dependencies_of SUBLIBRARY _retval_name)
    set(VISITED "")
    _compute_all_sublibrary_dependencies_of(${SUBLIBRARY} "" 0)
    list(SORT VISITED)
    set(${_retval_name} ${VISITED} PARENT_SCOPE)
endfunction()

# This is a private helper function for print_dependency_graph_of_sublibrary.
function(_print_dependency_graph_of_sublibrary SUBLIBRARY RECURSION_INDENT)
    get_target_property(_brief_doc_string ${SUBLIBRARY} BRIEF_DOC_STRING)
    verbose_message("${RECURSION_INDENT}${SUBLIBRARY} -- ${_brief_doc_string}")
    get_target_property(_explicit_sublibrary_dependencies ${SUBLIBRARY} EXPLICIT_SUBLIBRARY_DEPENDENCIES)
    foreach(_dep ${_explicit_sublibrary_dependencies})
        _print_dependency_graph_of_sublibrary(${_dep} "${RECURSION_INDENT}    ")
    endforeach()
endfunction()

# This function prints a dependency graph of the given sublibrary, simply using
# nested, indented text lines to denote dependency.
function(print_dependency_graph_of_sublibrary SUBLIBRARY)
    _print_dependency_graph_of_sublibrary(${SUBLIBRARY} "")
endfunction()

# This function prints a dependency graph for a library that explicitly depends on
# the sublibraries listed in LINK_SUBLIBRARIES.
function(print_dependency_graph_of_sublibrary_linking_library LIBNAME LINK_SUBLIBRARIES)
    verbose_message("${LIBNAME} -- depends explicitly on [${LINK_SUBLIBRARIES}]")
    foreach(_link_sublibrary ${LINK_SUBLIBRARIES})
        _print_dependency_graph_of_sublibrary(${_link_sublibrary} "    ")
    endforeach()
endfunction()

# TODO: write a dot graph generator which produces the dependency graph.

###################################################################################################
# Test functions
###################################################################################################

macro(check_deps SUBLIBRARY_NAME EXPECTED_DEPS)
    compute_all_sublibrary_dependencies_of(${SUBLIBRARY_NAME} DEPS)
    if("${DEPS}" STREQUAL "${EXPECTED_DEPS}")
        # message("dependencies of ${SUBLIBRARY_NAME} = ${DEPS} -- got expected value")
    else()
        message(SEND_ERROR "dependencies of ${SUBLIBRARY_NAME} = ${DEPS} -- expected ${EXPECTED_DEPS}")
    endif()
endmacro()

function(define_test_sublibrary SUBLIBRARY EXPLICIT_SUBLIBRARY_DEPENDENCIES)
    add_custom_target(${SUBLIBRARY})
    set_target_properties(${SUBLIBRARY} PROPERTIES EXPLICIT_SUBLIBRARY_DEPENDENCIES "${EXPLICIT_SUBLIBRARY_DEPENDENCIES}")
endfunction()

function(test_compute_all_sublibrary_dependencies_of)
    # Mutually-dependending sublibraries.
    define_test_sublibrary(A "B")
    define_test_sublibrary(B "A")
    check_deps(A "A;B")
    check_deps(B "A;B")

    # Self-dependending sublibrary (it's not necessary to specify self-dependency,
    # but it shouldn't hurt either).
    define_test_sublibrary(O "O")
    check_deps(O "O")

    # A 3-cycle of dependency.
    define_test_sublibrary(P "Q")
    define_test_sublibrary(Q "R")
    define_test_sublibrary(R "P")
    check_deps(P "P;Q;R")
    check_deps(Q "P;Q;R")
    check_deps(R "P;Q;R")

    # A diamond of dependency -- the more-northern sublibraries depend
    # on each more-southern sublibraries.
    define_test_sublibrary(N "W;E")
    define_test_sublibrary(W "S")
    define_test_sublibrary(E "S")
    define_test_sublibrary(S "")
    check_deps(N "E;N;S;W")
    check_deps(W "S;W")
    check_deps(E "E;S")
    check_deps(S "S")

    # TODO: make tests for other complicated graph cases?
endfunction()

# test_compute_all_sublibrary_dependencies_of()
