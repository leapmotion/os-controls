add_sublibrary(
    GLTexture2FreeImage
    SOURCE_PATH
        .
    HEADERS
        GLTexture2FreeImage.h
    SOURCES
        GLTexture2FreeImage.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLTexture2
    EXPLICIT_LIBRARY_DEPENDENCIES
        "FreeImage 3"
    BRIEF_DOC_STRING
        "Functionality for creating a GLTexture2 from an image file via FreeImage."
)
