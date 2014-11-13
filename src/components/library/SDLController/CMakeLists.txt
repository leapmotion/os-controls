# NOTE: MakeTransparent_Apple.mm is an Objective-C++ source file which should only be compiled
# on Apple platforms.  It should be ignored on other platforms.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(_maybe_MakeTransparent_Apple_mm "MakeTransparent_Apple.mm")
endif()

add_sublibrary(
    SDLController
    HEADERS
        SDLController.h
    SOURCES
        SDLController.cpp
        ${_maybe_MakeTransparent_Apple_mm}
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        GLCompatibility
    EXPLICIT_LIBRARY_DEPENDENCIES
        "SDL 2"
    BRIEF_DOC_STRING
        "A single point of control for init, shutdown, and general use of SDL."
)