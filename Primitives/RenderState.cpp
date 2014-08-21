#include "RenderState.h"

RenderState::RenderState() { }

void RenderState::UploadMatrices() {
  if (m_shader->HasUniform("normalMatrix")) {
    // upload normal matrix
    const Matrix4x4 result = (m_ModelView.Matrix()).inverse().transpose();
    glUniformMatrix4fv(m_shader->LocationOfUniform("normalMatrix"), 1, GL_FALSE, result.cast<float>().eval().data());
  }

  if (m_shader->HasUniform("projection")) {
    // upload projection matrix
    glUniformMatrix4fv(m_shader->LocationOfUniform("projection"), 1, GL_FALSE, m_Projection.Matrix().cast<float>().eval().data());
  }

  if (m_shader->HasUniform("modelView")) {
    // upload modelview matrix
    glUniformMatrix4fv(m_shader->LocationOfUniform("modelView"), 1, GL_FALSE, m_ModelView.Matrix().cast<float>().eval().data());
  }
}

void RenderState::UploadMaterial(const Color& diffuseColor, float ambientFactor, bool useTexture, GLint textureUnitIndex) {
  if (m_shader->HasUniform("diffuseColor")) {
    glUniform4f(m_shader->LocationOfUniform("diffuseColor"), diffuseColor.R(), diffuseColor.G(), diffuseColor.B(), diffuseColor.A());
  }
  if (m_shader->HasUniform("ambientFactor")) {
    glUniform1f(m_shader->LocationOfUniform("ambientFactor"), ambientFactor);
  }
  if (m_shader->HasUniform("useTexture")) {
    glUniform1i(m_shader->LocationOfUniform("useTexture"), useTexture);
  }
  if (m_shader->HasUniform("texture")) {
    glUniform1i(m_shader->LocationOfUniform("texture"), textureUnitIndex);
  }
}

void RenderState::EnablePositionAttribute() {
  if (!HavePositionAttribute()) {
    return;
  }
  GLint loc = m_shader->LocationOfAttribute("position");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, 0); // TODO: i think the GL_TRUE (for normalized) is wrong
}

void RenderState::EnableNormalAttribute() {
  if (!HaveNormalAttribute()) {
    return;
  }
  GLint loc = m_shader->LocationOfAttribute("normal");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::EnableColorAttribute() {
  if (!HaveColorAttribute()) {
    return;
  }
  GLint loc = m_shader->LocationOfAttribute("color");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::EnableTexCoordAttribute() {
  if (!HaveTexCoordAttribute()) {
    return;
  }
  GLint loc = m_shader->LocationOfAttribute("texCoord");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 2, GL_FLOAT, GL_TRUE, 0, 0);
}

void RenderState::DisablePositionAttribute() {
  if (!HavePositionAttribute()) {
    return;
  }
  glDisableVertexAttribArray(m_shader->LocationOfAttribute("position"));
}

void RenderState::DisableNormalAttribute() {
  if (!HaveNormalAttribute()) {
    return;
  }
  glDisableVertexAttribArray(m_shader->LocationOfAttribute("normal"));
}

void RenderState::DisableColorAttribute() {
  if (!HaveColorAttribute()) {
    return;
  }
  glDisableVertexAttribArray(m_shader->LocationOfAttribute("color"));
}

void RenderState::DisableTexCoordAttribute() {
  if (!HaveTexCoordAttribute()) {
    return;
  }
  glDisableVertexAttribArray(m_shader->LocationOfAttribute("texCoord"));
}
