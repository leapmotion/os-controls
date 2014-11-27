add_sublibrary(
    GLMatrices
    HEADERS
        ModelViewProjection.h
    SOURCES
        ModelViewProjection.cpp
    INTERNAL_DEPENDENCIES
        EigenTypes
    BRIEF_DOC_STRING
        "Provides manipulation of modelview and projection matrices for usage in shaders."
)