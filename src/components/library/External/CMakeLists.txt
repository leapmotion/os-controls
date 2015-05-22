add_sublibrary(
    NanoSVG
    SOURCE_PATH
        NanoSVG
    HEADERS
        LICENSE.txt
        nanosvg.h
        nanosvgrast.h
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "NanoSVG library for reading SVG files -- created by Mikko Mononen."
)

add_sublibrary(
    PolyPartition
    SOURCE_PATH
        PolyPartition
    HEADERS
        polypartition.h
    SOURCES
        polypartition.cpp
    INTERNAL_DEPENDENCIES
        C++11
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "PolyPartition library for triangulating polygons -- created by Ivan Fratric."
)

add_sublibrary(
    GTest
    EXCLUDE_FROM_ALL
    SOURCE_PATH
        GTest
    HEADERS
        gtest/gtest.h
    SOURCES
        gtest/gtest-all.cc
        gtest/gtest_main.cc
    COMPILE_DEFINITIONS
        GTEST_HAS_PTHREAD=0 # This is specifically for Linux -- threads are not necessary.
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Provides an easy way to include and link the Google Test framework."
)