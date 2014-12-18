add_sublibrary(
    SceneGraph
    HEADERS
        SceneGraphNode.h
        SceneGraphNodeProperties.h
        SceneGraphNodeProperty.h
        SceneGraphNodeValues.h
    INTERNAL_DEPENDENCIES
        C++11 # TODO: get rid of this dependency and just use the C++11 compile options directly.
        EigenTypes # TODO: get rid of Eigen dependency.
    BRIEF_DOC_STRING
        "The SceneGraph component provides a C++ implementation of the scene graph design pattern."
)

add_subdirectory(Test)
