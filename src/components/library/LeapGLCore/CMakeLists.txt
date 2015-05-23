if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(_cxx11_compiler_flags "-std=c++11")
endif()

add_sublibrary(
    LeapGLCore
    SOURCE_PATH
        "."
    HEADERS
        Leap/GL/BufferObject.h
        Leap/GL/ColorComponent.h
        Leap/GL/Common.h
        Leap/GL/Error.h
        Leap/GL/Exception.h
        Leap/GL/GLHeaders.h
        Leap/GL/Internal/ColorComponent.h
        Leap/GL/Internal/EigenTypes.h
        Leap/GL/Internal/Map.h
        Leap/GL/Internal/Meta.h
        Leap/GL/Internal/ShaderFrontend.h
        Leap/GL/Internal/ShaderUniform.h
        Leap/GL/Internal/Tuple.h
        Leap/GL/Internal/TypeMap.h
        Leap/GL/Internal/Typle.h
        Leap/GL/Internal/UniformTraits.h
        Leap/GL/Internal/UniformUploader.h
        Leap/GL/Internal/VertexAttribute.h
        Leap/GL/Mesh.h
        Leap/GL/MeshAssembler.h
        Leap/GL/MeshException.h
        Leap/GL/ModelView.h
        Leap/GL/Projection.h
        Leap/GL/ResourceBase.h
        Leap/GL/Rgb.h
        Leap/GL/Rgba.h
        Leap/GL/Shader.h
        Leap/GL/ShaderException.h
        Leap/GL/ShaderFrontend.h
        Leap/GL/ShaderMatrices.h
        Leap/GL/Texture2.h
        Leap/GL/Texture2Exception.h
        Leap/GL/Texture2Params.h
        Leap/GL/Texture2PixelData.h
        Leap/GL/VertexAttribute.h
        Leap/GL/VertexBufferObject.h
        Leap/GL/VertexBufferObjectException.h
    SOURCES
        Leap/GL/BufferObject.cpp
        Leap/GL/ModelView.cpp
        Leap/GL/Projection.cpp
        Leap/GL/Shader.cpp
        Leap/GL/ShaderMatrices.cpp
        Leap/GL/Texture2.cpp
        Leap/GL/Texture2Params.cpp
        Leap/GL/Texture2PixelData.cpp
    COMPILE_OPTIONS
        ${_cxx11_compiler_flags}
    EXTERNAL_DEPENDENCIES
        "Eigen 3.2.1" # TODO: Refactor matrix math stuff to not require Eigen.
        "Glew 1.9.0"
        "OpenGL 2.1" # It is necessary to link GL after Glew on Linux.
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "The LeapGL core component provides C++ abstractions for most of the commonly used OpenGL concepts."
)

add_subdirectory(Test)
