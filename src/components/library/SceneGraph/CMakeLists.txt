add_sublibrary(
    SceneGraph
    SOURCE_PATH
        .
    HEADERS
        SceneGraphNode.h
        SceneGraphNodeProperties.h
        SceneGraphNodeProperty.h
        SceneGraphNodeValues.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        EigenTypes
    BRIEF_DOC_STRING
        "Provides a C++ baseclass which implements the 'scene graph' design pattern (see Wikipedia)."
)
