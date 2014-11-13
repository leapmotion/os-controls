add_sublibrary(
    GLTexture2Loader
    SOURCE_PATH
        .
    HEADERS
        GLTexture2Loader.h
    SOURCES
        GLTexture2Loader.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2FreeImage
        ResourceManager
    BRIEF_DOC_STRING
        "Provides ResourceManager-managed loading of GLTexture2 objects using FreeImage."
)