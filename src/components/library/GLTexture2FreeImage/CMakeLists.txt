add_sublibrary(
    GLTexture2FreeImage
    HEADERS
        GLTexture2FreeImage.h
    SOURCES
        GLTexture2FreeImage.cpp
    INTERNAL_DEPENDENCIES
        GLTexture2
    EXTERNAL_DEPENDENCIES
        "FreeImage 3"
    BRIEF_DOC_STRING
        "Functionality for creating a GLTexture2 from an image file via FreeImage."
)
