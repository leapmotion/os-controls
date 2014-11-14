###################################################################################################
# Sublibrary definitions
###################################################################################################

include(AddSublibrary)

# Add all appropriate subdirectories:
file(GLOB all_subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(subdirectory ${all_subdirectories})
  set(curdir "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}")
  if(NOT ${subdirectory} STREQUAL "." AND EXISTS "${curdir}/CMakeLists.txt")
    add_subdirectory(${subdirectory})
  endif()
endforeach()

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

if (MSVC_USE_MT)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:LIBCMT")
endif()
