
add_sublibrary(
    Color
    SOURCE_PATH
        .
    HEADERS
        Color.h
    SOURCES
        Color.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        EigenTypes
    BRIEF_DOC_STRING
        "Provides RGBA color and some color-space conversions."
)