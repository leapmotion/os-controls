
add_sublibrary(
    Application
    SOURCE_PATH "."
    HEADERS
        Application.h
        EventHandler.h
        Renderable.h
        RenderableEventHandler.h
    SOURCES
        Application.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRING
        "A simple framework for an SDL-based application."
)