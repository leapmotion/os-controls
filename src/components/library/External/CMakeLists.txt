add_sublibrary(
    NanoSVG
    SOURCE_PATH
        ./NanoSVG
    HEADERS
        LICENSE.txt
        nanosvg.h
        nanosvgrast.h
    BRIEF_DOC_STRING
        "NanoSVG library for reading SVG files -- created by Mikko Mononen."
)

add_sublibrary(
    PolyPartition
    SOURCE_PATH
        ./PolyPartition
    HEADERS
        polypartition.h
    SOURCES
        polypartition.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    BRIEF_DOC_STRING
        "PolyPartition library for triangulating polygons -- created by Ivan Fratric."
)