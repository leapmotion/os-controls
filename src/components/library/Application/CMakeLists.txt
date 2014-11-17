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
    INTERNAL_DEPENDENCIES
        C++11
    EXTERNAL_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRING
        "A simple framework for an SDL-based application."
)