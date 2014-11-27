add_sublibrary(
    GLTexture2
    HEADERS
        GLTexture2.h
        GLTexture2Params.h
        GLTexture2PixelData.h
    SOURCES
        GLTexture2.cpp
        GLTexture2Params.cpp
        GLTexture2PixelData.cpp
    INTERNAL_DEPENDENCIES
        C++11
        GLCompatibility
    BRIEF_DOC_STRING
        "A C++ class for managing 2D textures in OpenGL."
)
add_subdirectory(Test)