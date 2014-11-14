add_sublibrary(
    GLCompatibility
    HEADERS
        gl_glext_glu.h
        GLError.h # This might really belong in a different component
    EXTERNAL_DEPENDENCIES
        "Glew 1.9.0"
        "OpenGL 2.1" # It is necessary to link GL after Glew on Linux.
    BRIEF_DOC_STRING
        "A means for including OpenGL (and related) headers in a platform-agnostic way."
)