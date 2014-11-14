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
# sublibrary via the add_sublibrary will append to it.  UNADDED_SUBLIBRARIES is a list of
# the target names of all the sublibraries that could not be defined due to unmet dependencies.
macro(begin_sublibrary_definitions)
    set(ADDED_SUBLIBRARIES "")
    set(UNADDED_SUBLIBRARIES "")
endmacro()

# This function defines a sublibrary (logical subgrouping of source, as described above)
# as a library target.  The function uses the CMakeParseArguments paradigm, where all-uppercase
# keywords indicate the meaning of the arguments that follow it (e.g. the install cmake command).
# The options are as follows:
#
# - Optional boolean arguments (an option's presence enables that argument, and its non-presence
#   implicitly disables that argument -- this is the default):
#   * EXCLUDE_FROM_ALL -- Excludes this sublibrary's target from the "make all" target.  Does
#     not apply to interface-only sublibraries (see below at INTERFACE_IS_INTERFACE_ONLY).
#   * REQUIRED -- Indicates that this sublibrary (and therefore all of its dependencies) is required
#     by the build, and any failure to define its target and link dependencies should result in
#     an error.  This flag will also be set if the global REQUIRE_${SUBLIBRARY_NAME} flag is set.
# - Parameters taking a single argument:
#   * BRIEF_DOC_STRING <string> -- A brief description of this sublibrary which should fit within
#     one line (about 80 chars).
#   * SOURCE_PATH -- The path, relative to CMAKE_CURRENT_SOURCE_DIR, containing all the sublibrary
#     headers and sources.  If this value is left unspecified, then it will default to the sublibrary
#     name.
# - Parameters taking multiple arguments (each one is optional, unless otherwise specified):
#   * HEADERS [header1 [header2 [...]]] -- The list of headers for the sublibrary.  Each of these
#     should be specified using a relative path, based at the sublibrary's subdirectory.
#   * SOURCES [source1 [source2 [...]]] -- Analogous to HEADERS, but for source files.
#   * RESOURCES [resource1 [resource2 [...]]] -- Additional non-compiled files which will be
#     copied from the source path into the ${PROJECT_BINARY_DIR}/resources/ directory upon build.
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
# Node that the INTERFACE_ prefix is only required for INTERFACE targets (e.g. HEADERS or IS_PHONY
# would be an allowable target property name on a non-interface target), but uniformity in the
# target property names was desired.
# - INTERFACE_BRIEF_DOC_STRING                    -- As described above.
# - INTERFACE_DETAILED_DOC_STRINGS                -- As described above.

function(add_sublibrary SUBLIBRARY_NAME)
    verbose_message("add_sublibrary(${SUBLIBRARY_NAME} ...)")

    # Do the fancy map-style parsing of the arguments
    set(_options
        EXCLUDE_FROM_ALL
        REQUIRED
    )
    set(_one_value_args
        SOURCE_PATH             # Optional specification of relative path to headers and sources.
        BRIEF_DOC_STRING        # A one-line, short (no more than about 80 chars) description of the sublibrary.
    )
    set(_multi_value_args
        HEADERS
        SOURCES
        RESOURCES
        COMPILE_DEFINITIONS
        # COMPILE_FEATURES      # target_compile_features is a CMake 3.1 feature and should be added when we upgrade
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

    # Add the REQUIRE_${SUBLIBRARY_NAME} option for use in dependency checking.
    option(REQUIRE_${SUBLIBRARY_NAME} "${_arg_BRIEF_DOC_STRING}" OFF)

    # Parse the arguments for use in the following target-defining calls.
    if(_arg_EXCLUDE_FROM_ALL)
        set(_exclude_from_all "EXCLUDE_FROM_ALL")
    else()
        set(_exclude_from_all "")
    endif()

    # Determine the directory for the sublibrary sources.
    if(_arg_SOURCE_PATH)
        verbose_message("    using explicitly-specified SOURCE_PATH (${_arg_SOURCE_PATH}) for SOURCE_PATH")
        set(_sublibrary_source_path ${_arg_SOURCE_PATH})
    else()
        verbose_message("    using SUBLIBRARY_NAME (${SUBLIBRARY_NAME}) for SOURCE_PATH")
        set(_sublibrary_source_path ${SUBLIBRARY_NAME})
    endif()
    
    if(_arg_REQUIRED OR REQUIRE_${SUBLIBRARY_NAME})
        set(_required TRUE)
    else()
        set(_required FALSE)
    endif()
    # Check for the existence of the sublibrary dependencies.  If any don't exist as targets,
    # don't define this sublibrary as a target.  Same with library dependencies.  That way,
    # missing library dependencies propagate the blocking of sublibrary definitions all the
    # way up.  This will produce a warning though.
    if(_required)
        set(_unmet_dependency_message_status SEND_ERROR)
    else()
        set(_unmet_dependency_message_status STATUS)
    endif()
    foreach(_dep ${_arg_EXPLICIT_SUBLIBRARY_DEPENDENCIES})
        verbose_message("checking if sublibrary ${_dep} can be depended upon by sublibrary ${SUBLIBRARY_NAME}.")
            message(${_unmet_dependency_message_status} "The \"${SUBLIBRARY_NAME}\" sublibrary has unmet sublibrary dependency ${_dep}, and therefore can't be defined.  This may be an inteded behavior (for example if you legitimately lack a library dependency and don't want the dependent sublibraries to be built) or may indicate a real error in the sublibrary definition.")
            return()
        if(NOT TARGET ${_dep})
            set(UNADDED_SUBLIBRARIES ${UNADDED_SUBLIBRARIES} ${SUBLIBRARY_NAME} PARENT_SCOPE)
        endif()
        verbose_message("it can be -- proceeding with target definition as normal.")
    endforeach()
    foreach(_dep ${_arg_EXPLICIT_LIBRARY_DEPENDENCIES})
        # For each library that hasn't been target_package'ed yet, call target_package on it.
        separate_arguments(_dep)
        list(GET _dep 0 _lib_name)
        set(_lib_target_name ${_lib_name}::${_lib_name})
        verbose_message("checking if package ${_lib_target_name} or ${_lib_name} can be depended upon by sublibrary ${SUBLIBRARY_NAME}")
        if(NOT TARGET ${_lib_target_name})
            verbose_message("calling find_package(${_dep})")
            find_package(${_dep})
            if(NOT TARGET ${_lib_target_name})
                message(${_unmet_dependency_message_status} "The \"${SUBLIBRARY_NAME}\" sublibrary has unmet library dependency \"${_dep}\", and therefore can't be defined.  This may be an inteded behavior (for example if you legitimately lack a library dependency and don't want the dependent sublibraries to be built) or may indicate a real error in the sublibrary definition.")
                set(UNADDED_SUBLIBRARIES ${UNADDED_SUBLIBRARIES} ${SUBLIBRARY_NAME} PARENT_SCOPE)
                return()
            endif()
        endif()
        verbose_message("it can be -- proceeding with target definition as normal.")
    endforeach()

    # Determine the relative paths of all the headers.
    set(_path_prefixed_headers "")
    foreach(_header ${_arg_HEADERS})
        list(APPEND _path_prefixed_headers ${_sublibrary_source_path}/${_header})
    endforeach()
    # Determine the relative paths of all the sources.
    set(_path_prefixed_sources "")
    foreach(_source ${_arg_SOURCES})
        list(APPEND _path_prefixed_sources ${_sublibrary_source_path}/${_source})
    endforeach()
    # Determine the relative paths of all the resources.
    set(_path_prefixed_resources "")
    foreach(_resource ${_arg_RESOURCES})
        list(APPEND _path_prefixed_resources ${_sublibrary_source_path}/${_resource})
    endforeach()

    # TODO: Consider using `add_library(target OBJECT ...)` to make a library target
    # which doesn't compile down to an archived library, but otherwise behaves as one.
    # This may have caveats, such as calling
    #   add_executable(user_app $<TARGET_OBJECTS:target>)
    # (essentially treating it as a set of sources) instead of
    #   target_link_libraries(user_app target)
    # See the docs for add_library.
    list(LENGTH _arg_SOURCES _source_count)
    list(LENGTH _arg_HEADERS _headers_count)
    if(${_source_count} EQUAL 0 AND ${_headers_count} EQUAL 0)
        set(_is_interface_only TRUE)
        add_library(${SUBLIBRARY_NAME} INTERFACE)
        set(_target_scope INTERFACE)
    elseif(${_source_count} EQUAL 0)
        set(_is_interface_only TRUE)
        add_library(${SUBLIBRARY_NAME} STATIC ${_exclude_from_all} ${_path_prefixed_headers} ${_path_prefixed_sources})
        set_property(TARGET ${SUBLIBRARY_NAME} PROPERTY LINKER_LANGUAGE CXX)
        set(_target_scope PUBLIC)
    else()
        set(_is_interface_only FALSE)
        add_library(${SUBLIBRARY_NAME} ${_exclude_from_all} ${_path_prefixed_headers} ${_path_prefixed_sources})
        # This is the scope specifier for use in the target_* functions called on this target.
        set(_target_scope PUBLIC)
    endif()

    if(NOT ${_target_scope} STREQUAL INTERFACE)
        set_target_properties(${SUBLIBRARY_NAME} PROPERTIES FOLDER Components)
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
            ${SUBLIBRARY_NAME}
            ${_target_scope}
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_sublibrary_source_path}>
                $<INSTALL_INTERFACE:include/${_sublibrary_source_path}>)
    endif()
    # If there are compile definitions, add them.
    if(_arg_COMPILE_DEFINITIONS)
        target_compile_definitions(${SUBLIBRARY_NAME} ${_target_scope} ${_arg_COMPILE_DEFINITIONS})
    endif()
    # If there are compile options, add them.
    if(_arg_COMPILE_OPTIONS)
        target_compile_options(${SUBLIBRARY_NAME} ${_target_scope} ${_arg_COMPILE_OPTIONS})
    endif()

    # Add link libraries from each sublibrary dependency.  The target_link_directories
    # command sets up the propagation of the various INTERFACE_XXX target properties
    # (e.g. INTERFACE_INCLUDE_DIRECTORIES, INTERFACE_COMPILE_OPTIONS,
    # INTERFACE_LINK_LIBRARIES) during build time from the dependencies to their
    # dependents.
    foreach(_dep ${_arg_EXPLICIT_SUBLIBRARY_DEPENDENCIES})
        target_link_libraries(${SUBLIBRARY_NAME} ${_target_scope} ${_dep})
    endforeach()

    # Add include directories and link libraries from each library dependency,
    # analogously to that of the sublibrary dependencies.  This requires calling
    # find_package on the libraries which haven't been loaded as targets yet.
    foreach(_dep ${_arg_EXPLICIT_LIBRARY_DEPENDENCIES})
        # For each library that hasn't been target_package'ed yet, call target_package on it.
        string(REPLACE " " ";" _semicolon_delimited_dep ${_dep})
        list(GET _semicolon_delimited_dep 0 _lib_name)
        set(_lib_target_name ${_lib_name}::${_lib_name})
        target_link_libraries(${SUBLIBRARY_NAME} ${_target_scope} ${_lib_target_name})
    endforeach()

    # Define post-build rules for copying resources.
    foreach(_resource ${_arg_RESOURCES})
        add_custom_command(
            TARGET ${SUBLIBRARY_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${_sublibrary_source_path}/${_resource}" "${PROJECT_BINARY_DIR}/resources/${_resource}"
        )
    endforeach()
    
    # Store several of the parameter values as target properties
    set_target_properties(
        ${SUBLIBRARY_NAME}
        PROPERTIES
            INTERFACE_BRIEF_DOC_STRING "${_arg_BRIEF_DOC_STRING}"
            INTERFACE_DETAILED_DOC_STRINGS "${_arg_DETAILED_DOC_STRINGS}"
    )

    # Add any other particular target properties.  NOTE: This should be done last, so it can override
    # any other property that has already been set.
    if(${_additional_target_property_count})
        set_target_properties(${SUBLIBRARY_NAME} PROPERTIES ${_arg_ADDITIONAL_TARGET_PROPERTIES})
    endif()

    # Append this sublibrary to the list of defined sublibraries.
    set(ADDED_SUBLIBRARIES ${ADDED_SUBLIBRARIES} ${SUBLIBRARY_NAME} PARENT_SCOPE)
endfunction()

# This is a private helper function for print_dependency_graph_of_sublibrary.
function(_print_dependency_graph_of_target TARGET RECURSION_INDENT)
    get_target_property(_brief_doc_string ${TARGET} INTERFACE_BRIEF_DOC_STRING)
    verbose_message("${RECURSION_INDENT}${SUBLIBRARY} -- ${_brief_doc_string}")
    get_target_property(_link_libraries ${TARGET} INTERFACE_LINK_LIBRARIES)
    foreach(_dep ${_link_libraries})
        _print_dependency_graph_of_target(${_dep} "${RECURSION_INDENT}    ")
    endforeach()
endfunction()

# This function prints a dependency graph of the given sublibrary, simply using
# nested, indented text lines to denote dependency.
function(print_dependency_graph_of_target SUBLIBRARY)
    _print_dependency_graph_of_target(${SUBLIBRARY} "")
endfunction()

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
