#include "RenderState.h"

RenderState::RenderState() :
  m_PositionAttribute(-1),
  m_NormalAttribute(-1),
  m_ColorAttribute(-1),
  m_TexCoordAttribute(-1),
  m_ModelViewMatrixUniform(-1),
  m_ProjectionMatrixUniform(-1),
  m_NormalMatrixUniform(-1),
  m_DiffuseColorUniform(-1),
  m_AmbientFactorUniform(-1)
{

}

void RenderState::UploadMatrices() {
  if (m_NormalMatrixUniform >= 0) {
    // upload normal matrix
    const Matrix4x4 result = (m_ModelView.Matrix()).inverse().transpose();
    glUniformMatrix4fv(m_NormalMatrixUniform, 1, GL_FALSE, result.cast<float>().eval().data());
  }

  if (m_ProjectionMatrixUniform >= 0) {
    // upload projection matrix
    glUniformMatrix4fv(m_ProjectionMatrixUniform, 1, GL_FALSE, m_Projection.Matrix().cast<float>().eval().data());
  }

  if (m_ModelViewMatrixUniform >= 0) {
    // upload modelview matrix
    glUniformMatrix4fv(m_ModelViewMatrixUniform, 1, GL_FALSE, m_ModelView.Matrix().cast<float>().eval().data());
  }
}

void RenderState::UploadMaterial(const Color& diffuseColor, float ambientFactor) {
  if (m_DiffuseColorUniform >= 0) {
    glUniform4f(m_DiffuseColorUniform, diffuseColor.R(), diffuseColor.G(), diffuseColor.B(), diffuseColor.A());
  }
  if (m_AmbientFactorUniform >= 0) {
    glUniform1f(m_AmbientFactorUniform, ambientFactor);
  }
}

void RenderState::EnablePositionAttribute() {
  glEnableVertexAttribArray(m_PositionAttribute);
  glVertexAttribPointer(m_PositionAttribute, 3, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::EnableNormalAttribute() {
  glEnableVertexAttribArray(m_NormalAttribute);
  glVertexAttribPointer(m_NormalAttribute, 3, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::EnableColorAttribute() {
  glEnableVertexAttribArray(m_ColorAttribute);
  glVertexAttribPointer(m_ColorAttribute, 3, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::EnableTexCoordAttribute() {
  glEnableVertexAttribArray(m_TexCoordAttribute);
  glVertexAttribPointer(m_TexCoordAttribute, 2, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::DisablePositionAttribute() {
  glDisableVertexAttribArray(m_PositionAttribute);
}

void RenderState::DisableNormalAttribute() {
  glDisableVertexAttribArray(m_NormalAttribute);
}

void RenderState::DisableColorAttribute() {
  glDisableVertexAttribArray(m_ColorAttribute);
}

void RenderState::DisableTexCoordAttribute() {
  glDisableVertexAttribArray(m_TexCoordAttribute);
}
