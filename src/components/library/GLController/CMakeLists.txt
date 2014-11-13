add_sublibrary(
    GLController
    SOURCE_PATH
        .
    HEADERS
        GLController.h
    SOURCES
        GLController.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLCompatibility
    BRIEF_DOC_STRING
        "A single point of control for usage of OpenGL."
)