add_sublibrary(
    GLShader
    HEADERS
        GLShader.h
        GLShaderBindingScopeGuard.h
    SOURCES
        GLShader.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        GLCompatibility
        ScopeGuard
    BRIEF_DOC_STRING
        "A C++ class which manages GLSL-based shader programs."
)