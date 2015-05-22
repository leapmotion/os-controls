add_sublibrary(
    GLShaderLoader
    HEADERS
        GLShaderLoader.h
    INTERNAL_DEPENDENCIES
        LeapGLCore
        Resource
        TextAndBinaryFileLoader
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of GLSL-based shader programs."
)