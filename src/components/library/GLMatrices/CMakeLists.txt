add_sublibrary(
    GLMatrices
    SOURCE_PATH
        .
    HEADERS
        ModelViewProjection.h
    SOURCES
        ModelViewProjection.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        EigenTypes
    BRIEF_DOC_STRING
        "Provides manipulation of modelview and projection matrices for usage in shaders."
)