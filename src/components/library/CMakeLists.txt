include(AddSublibrary)

###################################################################################################
# Sublibrary definitions
###################################################################################################

add_subdirectory(Animation)
add_subdirectory(Application)
add_subdirectory(C++11)
add_subdirectory(EigenTypes)
add_subdirectory(External)
add_subdirectory(GLShaderLoader)
add_subdirectory(GLTestFramework)
add_subdirectory(GLTexture2FreeImage)
add_subdirectory(GLTexture2Image)
add_subdirectory(GLTexture2Loader)
add_subdirectory(HandCursor)
add_subdirectory(LeapGLCore)
add_subdirectory(LeapListener)
add_subdirectory(OculusVR)
add_subdirectory(Primitives)
add_subdirectory(RadialMenu)
add_subdirectory(Resource)
add_subdirectory(ResourceManager)
add_subdirectory(RiggedHand)
add_subdirectory(Rigging)
add_subdirectory(SceneGraph)
add_subdirectory(SDLController)
add_subdirectory(SFMLController)
add_subdirectory(Singleton)
add_subdirectory(TextAndBinaryFile)
add_subdirectory(TextAndBinaryFileLoader)

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

