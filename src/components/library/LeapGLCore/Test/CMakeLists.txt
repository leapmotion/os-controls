set(_sources
    ColorComponentTest.cpp
    ShaderTest.cpp
    Texture2Test.cpp
    ProjectionTest.cpp
    RgbaTest.cpp
    RgbTest.cpp
    ShaderFrontendTest.cpp
)
add_executable(LeapGLCoreTest ${_sources})
target_link_libraries(LeapGLCoreTest GLTestFramework GTest LeapGLCore)
set_property(TARGET LeapGLCoreTest PROPERTY FOLDER "Tests")
add_test(NAME LeapGLCoreTest COMMAND $<TARGET_FILE:LeapGLCoreTest>)
