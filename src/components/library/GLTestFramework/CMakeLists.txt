add_sublibrary(
    GLTestFramework
    EXCLUDE_FROM_ALL
    HEADERS
        GLTestFramework.h
    SOURCES
        GLTestFramework.cpp
    INTERNAL_DEPENDENCIES
        GLController
        GTest
        SDLController
    BRIEF_DOC_STRING
        "Provides an easy way to create GL-based tests."
)
