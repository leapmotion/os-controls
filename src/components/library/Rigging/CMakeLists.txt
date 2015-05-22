if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(_unicode_compiler_flags "/D UNICODE")
endif()

add_sublibrary(
    Rigging
    HEADERS
        AMeshSection.h
        ModelIo.h
        ModelSourceAssimp.h
        ModelTargetSkinnedVboMesh.h
        Node.h
        Skeleton.h
        SkinnedVboMesh.h
    SOURCES
        ModelIo.cpp
        ModelSourceAssimp.cpp
        ModelTargetSkinnedVboMesh.cpp
        Node.cpp
        Skeleton.cpp
        SkinnedVboMesh.cpp
    COMPILE_OPTIONS
        ${_unicode_compiler_flags}
    INTERNAL_DEPENDENCIES
        EigenTypes
        Primitives
        LeapGLCore
        GLTexture2Image
    EXTERNAL_DEPENDENCIES
        "Assimp 3.1.1"
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Mesh rigging, skinning, and deformation."
)