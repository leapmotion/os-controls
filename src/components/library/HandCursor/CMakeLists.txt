add_sublibrary(
    HandCursor
    HEADERS
        HandCursor.h
    SOURCES
        HandCursor.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        Color
        Animation
        EigenTypes
        GLCompatibility
        GLMaterial
        LeapListener
        Primitives
    BRIEF_DOC_STRING
        "Simple hand cursor representation."
)
