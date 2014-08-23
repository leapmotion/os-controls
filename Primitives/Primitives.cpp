#include "Primitives.h"

#include "GLTexture2.h"

void GenericShape::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized
  
  m_geometry.Draw(*m_shader, m_drawMode);

  modelView.Pop();
}

Sphere::Sphere() : m_Radius(1) { }

void Sphere::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitSphere(30);

  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3::Constant(m_Radius));

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized
  
  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

Cylinder::Cylinder() : m_Radius(1), m_Height(1) { }

void Cylinder::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitCylinder(50, 1);

  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3(m_Radius, m_Height, m_Radius));

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized

  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

Box::Box() : m_Size(Vector3::Constant(1.0)) { }

void Box::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitBox();

  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(m_Size);

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized

  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

Disk::Disk() : m_Radius(1) { }

void Disk::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitDisk(75);

  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3::Constant(m_Radius));

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized

  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

RectanglePrim::RectanglePrim() : m_Size(1, 1) { }

void RectanglePrim::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitSquare();

  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3(m_Size.x(), m_Size.y(), 1.0));

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized
  
  bool useTexture = bool(m_texture); // If there is a valid texture, enable texturing.

  if (useTexture) {
    glEnable(GL_TEXTURE_2D);
    m_texture->Bind();
  }
  geom.Draw(*m_shader, GL_TRIANGLES);
  if (useTexture) {
    glDisable(GL_TEXTURE_2D);
    m_texture->Unbind();
  }
  
  modelView.Pop();
}

PartialDisk::PartialDisk() : m_RecomputeGeometry(true), m_InnerRadius(0.5), m_OuterRadius(1), m_StartAngle(0), m_EndAngle(2*M_PI) { }

void PartialDisk::Draw(RenderState& renderState) const {
  if (m_InnerRadius >= m_OuterRadius || m_StartAngle >= m_EndAngle) {
    // don't proceed if the shape is empty
    return;
  }

  if (m_RecomputeGeometry) {
    RecomputeGeometry();
  }

  ModelView& modelView = renderState.GetModelView();

  modelView.Push();

  m_shader_matrices.SetMatrices(modelView.Matrix(), renderState.GetProjection().Matrix());
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  m_material.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND); // this could be optimized

  m_Geometry.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

void PartialDisk::RecomputeGeometry() const {
  double sweepAngle = m_EndAngle - m_StartAngle;
  if (sweepAngle > 2*M_PI) {
    sweepAngle = 2*M_PI;
  }

  static const double DESIRED_ANGLE_PER_SEGMENT = 0.1; // radians
  const int numSegments = static_cast<int>(sweepAngle / DESIRED_ANGLE_PER_SEGMENT) + 1;
  const double anglePerSegment = sweepAngle / numSegments;

  m_Geometry.CleanUpBuffers();

  double curAngle = m_StartAngle;
  const double cosStart = std::cos(m_StartAngle);
  const double sinStart = std::sin(m_StartAngle);
  Vector3f prevInner(static_cast<float>(m_InnerRadius*cosStart), static_cast<float>(m_InnerRadius*sinStart), 0.0f);
  Vector3f prevOuter(static_cast<float>(m_OuterRadius*cosStart), static_cast<float>(m_OuterRadius*sinStart), 0.0f);
  for (int i=0; i<numSegments; i++) {
    curAngle += anglePerSegment;

    const double cosCur = std::cos(curAngle);
    const double sinCur = std::sin(curAngle);

    const Vector3f curInner(static_cast<float>(m_InnerRadius*cosCur), static_cast<float>(m_InnerRadius*sinCur), 0.0f);
    const Vector3f curOuter(static_cast<float>(m_OuterRadius*cosCur), static_cast<float>(m_OuterRadius*sinCur), 0.0f);

    m_Geometry.PushTri(prevInner, prevOuter, curOuter);
    m_Geometry.PushTri(curOuter, curInner, prevInner);

    prevInner = curInner;
    prevOuter = curOuter;
  }

  m_Geometry.UploadDataToBuffers();

  m_RecomputeGeometry = false;
}
