add_sublibrary(
    GLTestFramework
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        .
    HEADERS
        GLTestFramework.h
    SOURCES
        GLTestFramework.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLController
        GTest
        SDLController
    BRIEF_DOC_STRING
        "Provides an easy way to create GL-based tests."
)
