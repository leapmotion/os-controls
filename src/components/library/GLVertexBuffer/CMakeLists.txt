add_sublibrary(
    GLVertexBuffer
    SOURCE_PATH
        .
    HEADERS
        GLVertexAttribute.h
        GLVertexBuffer.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        GLBuffer
        GLCompatibility
    BRIEF_DOC_STRING
        "An encapsulation of the OpenGL vertex buffer object concept."
)