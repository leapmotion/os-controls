
add_sublibrary(
    FrameBufferObject
    SOURCE_PATH
        .
    HEADERS
        FrameBufferObject.h
        RenderBuffer.h
    SOURCES
        FrameBufferObject.cpp
        RenderBuffer.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2
        GLCompatibility
    BRIEF_DOC_STRING
        "Render to an OpenGL render buffer or texture."
)