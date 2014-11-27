add_sublibrary(
    FrameBufferObject
    HEADERS
        FrameBufferObject.h
        RenderBuffer.h
    SOURCES
        FrameBufferObject.cpp
        RenderBuffer.cpp
    INTERNAL_DEPENDENCIES
        GLTexture2
        GLCompatibility
    BRIEF_DOC_STRING
        "Render to an OpenGL render buffer or texture."
)