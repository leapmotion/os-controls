add_sublibrary(
    LeapGLCoreTest
    EXCLUDE_FROM_ALL
    SOURCES
        ColorComponentTest.cpp
        GLShaderTest.cpp
        GLTexture2Test.cpp
        RgbaTest.cpp
        RgbTest.cpp
        SceneGraphNodePropertiesTest.cpp
        SceneGraphNodeTest.cpp
        ShaderFrontendTest.cpp
    INTERNAL_DEPENDENCIES
        GLTestFramework
        LeapGLCore
    BRIEF_DOC_STRING
        "Unit tests for the LeapGLCore component."
)