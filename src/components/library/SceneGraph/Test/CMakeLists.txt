set(_sources
    SceneGraphNodePropertiesTest.cpp
    SceneGraphNodeTest.cpp
)
add_executable(SceneGraphTest ${_sources})
target_link_libraries(SceneGraphTest GTest SceneGraph)
set_property(TARGET SceneGraphTest PROPERTY FOLDER "Tests")
add_test(NAME SceneGraphTest COMMAND $<TARGET_FILE:SceneGraphTest>)
