set(_sources
    ColorComponentTest.cpp
    GLShaderTest.cpp
    GLTexture2Test.cpp
    OrthographicCameraTest.cpp
    PerspectiveCameraTest.cpp
    RgbaTest.cpp
    RgbTest.cpp
    SceneGraphNodePropertiesTest.cpp
    SceneGraphNodeTest.cpp
    ShaderFrontendTest.cpp
)
add_executable(LeapGLCoreTest ${_sources})
target_link_libraries(LeapGLCoreTest GLTestFramework GTest LeapGLCore)
set_property(TARGET LeapGLCoreTest PROPERTY FOLDER "Tests")
add_test(NAME LeapGLCoreTest COMMAND $<TARGET_FILE:LeapGLCoreTest>)
