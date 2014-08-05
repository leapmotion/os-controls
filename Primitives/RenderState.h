#pragma once

#include "ModelViewProjection.h"
#include "Color.h"

class RenderState {

public:

  RenderState();

  // getters for modelview and projection
  const ModelView& GetModelView() const { return m_ModelView; }
  ModelView& GetModelView() { return m_ModelView; }
  const Projection& GetProjection() const { return m_Projection; }
  Projection& GetProjection() { return m_Projection; }

  // upload functions
  void UploadMatrices();
  void UploadMaterial(const Color& diffuseColor, float ambientFactor);

  // attributes
  void SetPositionAttribute(int attr) { m_PositionAttribute = attr; }
  void SetNormalAttribute(int attr) { m_NormalAttribute = attr; }
  void SetColorAttribute(int attr) { m_ColorAttribute = attr; }
  void SetTexCoordAttribute(int attr) { m_TexCoordAttribute = attr; }

  // matrix uniforms
  void SetModelViewMatrixUniform(int uniform) { m_ModelViewMatrixUniform = uniform; }
  void SetProjectionMatrixUniform(int uniform) { m_ProjectionMatrixUniform = uniform; }
  void SetNormalMatrixUniform(int uniform) { m_NormalMatrixUniform = uniform; }

  // material uniforms
  void SetAmbientFactorUniform(int uniform) { m_AmbientFactorUniform = uniform; }
  void SetDiffuseColorUniform(int uniform) { m_DiffuseColorUniform = uniform; }

  // attribute controls
  void EnablePositionAttribute();
  void EnableNormalAttribute();
  void EnableColorAttribute();
  void EnableTexCoordAttribute();
  void DisablePositionAttribute();
  void DisableNormalAttribute();
  void DisableColorAttribute();
  void DisableTexCoordAttribute();

  bool HavePositionAttribute() const { return m_PositionAttribute >= 0; }
  bool HaveNormalAttribute() const { return m_NormalAttribute >= 0; }
  bool HaveColorAttribute() const { return m_ColorAttribute >= 0; }
  bool HaveTexCoordAttribute() const { return m_TexCoordAttribute >= 0; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  // attributes
  int m_PositionAttribute;
  int m_NormalAttribute;
  int m_ColorAttribute;
  int m_TexCoordAttribute;

  // matrix uniforms
  int m_ModelViewMatrixUniform;
  int m_ProjectionMatrixUniform;
  int m_NormalMatrixUniform;

  // material
  int m_DiffuseColorUniform;
  int m_AmbientFactorUniform;

  // modelview and projection
  ModelView m_ModelView;
  Projection m_Projection;
};
