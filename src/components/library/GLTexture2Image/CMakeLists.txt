add_sublibrary(
    GLTexture2Image
    HEADERS
        GLTexture2Image.h
    SOURCES
        GLTexture2Image.cpp
    INTERNAL_DEPENDENCIES
        LeapGLCore
        GLTexture2FreeImage
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Wrapper from file to GL texture."
)