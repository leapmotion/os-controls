set(SFML_STATIC_LIBRARIES ON) #required for proper linkage..
add_sublibrary(
    SFMLController
    HEADERS
        SFMLController.h
    SOURCES
        SFMLController.cpp
    INTERNAL_DEPENDENCIES
        C++11
    EXTERNAL_DEPENDENCIES
        "SFML 2.1"
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "A single point of control for init, shutdown, and general use of SFML."
)
if(APPLE)
  set_property(SOURCE SFMLController.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
endif()
