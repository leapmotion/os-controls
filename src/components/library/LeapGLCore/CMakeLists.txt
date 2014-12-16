add_sublibrary(
    LeapGLCore
    HEADERS
        Leap/GL/ColorComponent.h
        Leap/GL/Common.h
        Leap/GL/Exception.h
        Leap/GL/BufferObject.h
        Leap/GL/Error.h
        Leap/GL/GLHeaders.h
        Leap/GL/Internal/ColorComponent.h
        Leap/GL/Internal/Map.h
        Leap/GL/Internal/Meta.h
        Leap/GL/Internal/ShaderFrontend.h
        Leap/GL/Internal/ShaderUniform.h
        Leap/GL/Internal/Tuple.h
        Leap/GL/Internal/TypeMap.h
        Leap/GL/Internal/Typle.h
        Leap/GL/Internal/UniformTraits.h
        Leap/GL/Internal/UniformUploader.h
        Leap/GL/Mesh.h
        Leap/GL/MeshException.h
        Leap/GL/ModelView.h
        Leap/GL/Projection.h
        Leap/GL/ResourceBase.h
        Leap/GL/Rgb.h
        Leap/GL/Rgba.h
        Leap/GL/SceneGraphNode.h
        Leap/GL/SceneGraphNodeProperties.h
        Leap/GL/SceneGraphNodeProperty.h
        Leap/GL/SceneGraphNodeValues.h
        Leap/GL/Shader.h
        Leap/GL/ShaderException.h
        Leap/GL/ShaderFrontend.h
        Leap/GL/ShaderMatrices.h
        Leap/GL/Texture2.h
        Leap/GL/Texture2Exception.h
        Leap/GL/Texture2Params.h
        Leap/GL/Texture2PixelData.h
        Leap/GL/VertexAttribute.h
        Leap/GL/VertexBuffer.h
        Leap/GL/VertexBufferException.h
    SOURCES
        Leap/GL/BufferObject.cpp
        Leap/GL/ModelView.cpp
        Leap/GL/Projection.cpp
        Leap/GL/Shader.cpp
        Leap/GL/ShaderMatrices.cpp
        Leap/GL/Texture2.cpp
        Leap/GL/Texture2Params.cpp
        Leap/GL/Texture2PixelData.cpp
    INTERNAL_DEPENDENCIES
        C++11 # TODO: get rid of this dependency and just use the C++11 compile options directly.
        EigenTypes # TODO: get rid of Eigen dependency.
    EXTERNAL_DEPENDENCIES
        "Glew 1.9.0"
        "OpenGL 2.1" # It is necessary to link GL after Glew on Linux.
    BRIEF_DOC_STRING
        "The LeapGL core component provides C++ abstractions for most of the commonly used OpenGL concepts."
)

add_subdirectory(Test)
