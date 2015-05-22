add_sublibrary(
    GLTexture2Loader
    HEADERS
        GLTexture2Loader.h
    SOURCES
        GLTexture2Loader.cpp
    INTERNAL_DEPENDENCIES
        GLTexture2FreeImage
        ResourceManager
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Provides ResourceManager-managed loading of Texture2 objects using FreeImage."
)