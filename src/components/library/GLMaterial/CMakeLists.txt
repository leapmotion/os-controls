add_sublibrary(
    GLMaterial
    HEADERS
        GLMaterial.h
        GLShaderMatrices.h
    SOURCES
        GLMaterial.cpp
        GLShaderMatrices.cpp
    RESOURCES
        material-frag.glsl
        matrix-transformed-vert.glsl
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        Color
        EigenTypes
        GLShader
    BRIEF_DOC_STRING
        "A C++ class which provides high-level management of a 'material' shader."
)
