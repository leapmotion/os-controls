add_executable(SceneGraphTest SceneGraphNodeTest.cpp SceneGraphNodePropertiesTest.cpp)
target_link_libraries(SceneGraphTest SceneGraph GTest)
set_property(TARGET SceneGraphTest PROPERTY FOLDER "Tests")
add_test(NAME SceneGraphTest COMMAND $<TARGET_FILE:SceneGraphTest>)
