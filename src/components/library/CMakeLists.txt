include(AddSublibrary)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(_unicode_compiler_flags "/D UNICODE")
endif()

add_sublibrary(
    Unicode
    COMPILE_OPTIONS
        ${_unicode_compiler_flags}
    BRIEF_DOC_STRING
        "A phony target which just provides compiler options for enabling Unicode support."
)

)

add_sublibrary(
    GLTexture2Image
    HEADERS
        GLTexture2Image.h
    SOURCES
        GLTexture2Image.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2
        GLTexture2FreeImage
    BRIEF_DOC_STRING
        "Wrapper from file to GL texture."
)

add_sublibrary(
    Rigging
    HEADERS
        AMeshSection.h
        ModelIo.h
        ModelSourceAssimp.h
        ModelTargetSkinnedVboMesh.h
        Node.h
        Skeleton.h
        SkinnedVboMesh.h
    SOURCES
        ModelIo.cpp
        ModelSourceAssimp.cpp
        ModelTargetSkinnedVboMesh.cpp
        Node.cpp
        Skeleton.cpp
        SkinnedVboMesh.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        Color
        EigenTypes
        Primitives
        GLShader
        GLTexture2Image
        GLVertexBuffer
        Unicode
    EXPLICIT_LIBRARY_DEPENDENCIES
        "Assimp 3.1.1"
    BRIEF_DOC_STRING
        "Mesh rigging, skinning, and deformation."
)

add_sublibrary(
    RiggedHand
    HEADERS
        RiggedHand.h
    SOURCES
        RiggedHand.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        Rigging
    BRIEF_DOC_STRING
        "Realistic deformable hand."
###################################################################################################
# Sublibrary definitions
###################################################################################################

leap_add_all_subdirectories()

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

