#pragma once

#include "EigenTypes.h"
#include "Leap/GL/ShaderFrontend.h"

namespace Leap {
namespace GL {

class Shader;

// Presents an interface specifically tailored to shaders having [optional] matrix uniforms of the form:
//   uniform mat4 projection_times_model_view_matrix
//   uniform mat4 model_view_matrix
//   uniform mat4 normal_matrix
// These quantities are derived from the model view matrix and the projection matrix.
class ShaderMatrices {
public:

  // Leaving any of the matrix names empty will cause the corresponding uniforms to go unused.
  ShaderMatrices (const Shader &shader,
                  const std::string &projection_times_model_view_matrix_id,
                  const std::string &model_view_matrix_id,
                  const std::string &normal_matrix_id);

  void SetMatrices (const EigenTypes::Matrix4x4 &model_view, const EigenTypes::Matrix4x4 &projection);
  void UploadUniforms ();

private:

  enum class ShaderMatrix {
    PROJECTION_TIMES_MODEL_VIEW,
    MODEL_VIEW,
    NORMAL
  };

  template <ShaderMatrix NAME_, GLenum GL_TYPE_, typename CppType_, MatrixStorageConvention MATRIX_STORAGE_CONVENTION_>
  using ShaderMatrixUniform = Leap::GL::MatrixUniform<ShaderMatrix,NAME_,GL_TYPE_,CppType_,MATRIX_STORAGE_CONVENTION_>;

  typedef ShaderFrontend<ShaderMatrix,
                         ShaderMatrixUniform<ShaderMatrix::PROJECTION_TIMES_MODEL_VIEW,GL_FLOAT_MAT4,EigenTypes::Matrix4x4f,COLUMN_MAJOR>,
                         ShaderMatrixUniform<ShaderMatrix::MODEL_VIEW,GL_FLOAT_MAT4,EigenTypes::Matrix4x4f,COLUMN_MAJOR>,
                         ShaderMatrixUniform<ShaderMatrix::NORMAL,GL_FLOAT_MAT4,EigenTypes::Matrix4x4f,COLUMN_MAJOR>> Frontend;
  Frontend m_frontend;
};

} // end of namespace GL
} // end of namespace Leap
