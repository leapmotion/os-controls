add_sublibrary(
    GLShaderLoader
    HEADERS
        GLShaderLoader.h
    INTERNAL_DEPENDENCIES
        LeapGLCore
        Resource
        TextAndBinaryFileLoader
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of GLSL-based shader programs."
)