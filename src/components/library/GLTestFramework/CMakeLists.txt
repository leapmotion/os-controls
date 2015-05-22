add_sublibrary(
    GLTestFramework
    EXCLUDE_FROM_ALL
    HEADERS
        GLTestFramework.h
    SOURCES
        GLTestFramework.cpp
    INTERNAL_DEPENDENCIES
        LeapGLCore
        GTest
        SDLController
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Provides an easy way to create GL-based tests."
)
