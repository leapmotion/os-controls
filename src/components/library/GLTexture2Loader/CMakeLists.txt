add_sublibrary(
    GLTexture2Loader
    HEADERS
        GLTexture2Loader.h
    SOURCES
        GLTexture2Loader.cpp
    INTERNAL_DEPENDENCIES
        GLTexture2FreeImage
        ResourceManager
    BRIEF_DOC_STRING
        "Provides ResourceManager-managed loading of GLTexture2 objects using FreeImage."
)