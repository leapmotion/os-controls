
set(SFML_STATIC_LIBRARIES ON) #required for proper linkage..
add_sublibrary(
    SFMLController
    SOURCE_PATH
        .
    HEADERS
        SFMLController.h
    SOURCES
        SFMLController.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SFML 2.1"
    BRIEF_DOC_STRING
        "A single point of control for init, shutdown, and general use of SFML."
)
if(APPLE)
  set_property(SOURCE SFMLController/SFMLController.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
endif()