add_sublibrary(
    LeapListener
    HEADERS
        LeapListener.h
    SOURCES
        LeapListener.cpp
    INTERNAL_DEPENDENCIES
        C++11
    EXTERNAL_DEPENDENCIES
        "Leap 2.1"
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Provides a small but useful extension of the Leap::Listener class."
)
