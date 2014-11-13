add_sublibrary(
    GLShaderLoader
    SOURCE_PATH
        .
    HEADERS
        GLShaderLoader.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        GLShader
        Resource
        TextAndBinaryFileLoader
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of GLSL-based shader programs."
)