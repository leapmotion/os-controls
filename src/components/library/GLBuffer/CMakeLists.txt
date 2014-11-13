
add_sublibrary(
    GLBuffer
    HEADERS
        GLBuffer.h
    SOURCES
        GLBuffer.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLCompatibility
    BRIEF_DOC_STRING
        "A fast GPU buffer object for OpenGL geometry."
)