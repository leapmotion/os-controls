add_sublibrary(
    LeapListener
    HEADERS
        LeapListener.h
    SOURCES
        LeapListener.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "Leap 2.1"
    BRIEF_DOC_STRING
        "Provides a small but useful extension of the Leap::Listener class."
)
