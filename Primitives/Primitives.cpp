#include "Primitives.h"

#include "GLShaderBindingScopeGuard.h"
#include "GLTexture2.h"

void GenericShape::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();

  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.
  
  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  m_geometry.Draw(*m_shader, m_drawMode);

  modelView.Pop();
}

Sphere::Sphere() : m_Radius(1) { }

void Sphere::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3::Constant(m_Radius));
  
  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.

  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitSphere(30);
  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

Cylinder::Cylinder() : m_Radius(1), m_Height(1) { }

void Cylinder::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3(m_Radius, m_Height, m_Radius));
  
  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.

  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitCylinder(50, 1);
  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

Box::Box() : m_Size(Vector3::Constant(1.0)) { }

void Box::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(m_Size);
  
  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.

  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitBox();
  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

Disk::Disk() : m_Radius(1) { }

void Disk::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3::Constant(m_Radius));
  
  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.

  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitDisk(75);
  geom.Draw(*m_shader, GL_TRIANGLES);

  modelView.Pop();
}

RectanglePrim::RectanglePrim() : m_Size(1, 1) { }

void RectanglePrim::Draw(RenderState& renderState) const {
  ModelView& modelView = renderState.GetModelView();
  modelView.Push();
  modelView.Scale(Vector3(m_Size.x(), m_Size.y(), 1.0));
  
  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.

  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  bool useTexture = bool(m_texture); // If there is a valid texture, enable texturing.
  if (useTexture) {
    glEnable(GL_TEXTURE_2D);
    m_texture->Bind();
  }
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitSquare();
  geom.Draw(*m_shader, GL_TRIANGLES);
  if (useTexture) {
    glDisable(GL_TEXTURE_2D);
    m_texture->Unbind();
  }
  
  modelView.Pop();
}

ImagePrimitive::ImagePrimitive(const std::shared_ptr<GLTexture2> &texture) {
  if (!texture) {
    throw std::invalid_argument("ImagePrimitive: must specify a valid texture");
  }
  SetTexture(texture);
  SetScaleBasedOnTextureSize();
  Material().SetAmbientLightingProportion(1.0f);
  Material().SetUseTexture(true);
}

void ImagePrimitive::SetScaleBasedOnTextureSize () {
  SetSize(Vector2(Texture()->Params().Width(), Texture()->Params().Height()));
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
  
  GLShaderBindingScopeGuard bso(*m_shader, BindFlags::BIND_AND_UNBIND); // binds *m_shader now, unbinds upon end of scope.

  GLShaderMatrices::UploadUniforms(*m_shader, modelView.Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
  m_material.UploadUniforms(*m_shader, renderState.GetOpacityState().Opacity(), BindFlags::NONE);
  m_Geometry.Draw(*m_shader, GL_TRIANGLES);
  m_shader->Unbind();

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

PartialDiskWithTriangle::PartialDiskWithTriangle() : m_TriangleSide(OUTSIDE), m_TrianglePosition(0.5), m_TriangleWidth(0.1), m_TriangleOffset(0.35) {}

void PartialDiskWithTriangle::RecomputeGeometry() const {
  double sweepAngle = m_EndAngle - m_StartAngle;
  if (sweepAngle > 2*M_PI) {
    sweepAngle = 2*M_PI;
  }

  static const double DESIRED_ANGLE_PER_SEGMENT = 0.1; // radians
  int numSegments = static_cast<int>(sweepAngle / DESIRED_ANGLE_PER_SEGMENT) + 1;
  const double anglePerSegment = sweepAngle / numSegments;

  m_Geometry.CleanUpBuffers();

  double curAngle = m_StartAngle;
  const double cosStart = std::cos(m_StartAngle);
  const double sinStart = std::sin(m_StartAngle);
  Vector3f prevInner(static_cast<float>(m_InnerRadius*cosStart), static_cast<float>(m_InnerRadius*sinStart), 0.0f);
  Vector3f prevOuter(static_cast<float>(m_OuterRadius*cosStart), static_cast<float>(m_OuterRadius*sinStart), 0.0f);

  bool haveStarted = false;
  bool havePassedMidpoint = false;
  bool havePassedEnd = false;
  bool haveTakenCareOfExtraAngle = false;

  const double triangleAngle = sweepAngle * m_TriangleWidth;
  const double triangleStart = m_TrianglePosition * sweepAngle + m_StartAngle - triangleAngle / 2.0;
  const double triangleEnd = triangleStart + triangleAngle;
  const double triangleMidpoint = 0.5*(triangleStart + triangleEnd);

  while (curAngle < (m_EndAngle - 0.001)) {
    curAngle += anglePerSegment;

    if (!haveStarted && curAngle > triangleStart) {
      curAngle = triangleStart;
      haveStarted = true;
    } else if (!havePassedMidpoint && curAngle > triangleMidpoint) {
      curAngle = triangleMidpoint;
      havePassedMidpoint = true;
    } else if (!havePassedEnd && curAngle > triangleEnd) {
      curAngle = triangleEnd;
      havePassedEnd = true;
    } else if (havePassedEnd && !haveTakenCareOfExtraAngle) {
      haveTakenCareOfExtraAngle = true;
      curAngle = m_StartAngle + anglePerSegment * (static_cast<int>((curAngle-m_StartAngle) / anglePerSegment));
    }

    double innerRadius = m_InnerRadius;
    double outerRadius = m_OuterRadius;
    if (curAngle >= triangleStart && curAngle <= triangleEnd) {
      double ratio = (curAngle - triangleStart) / (triangleAngle);
      double mult = -2 * std::abs(ratio-0.5) + 1;
      const double triangleHeight = m_TriangleOffset * (m_OuterRadius - m_InnerRadius);
      if (m_TriangleSide == INSIDE) {
        innerRadius -= mult * triangleHeight;
      } else if (m_TriangleSide == OUTSIDE) {
        outerRadius += mult * triangleHeight;
      }
    }

    const double cosCur = std::cos(curAngle);
    const double sinCur = std::sin(curAngle);

    const Vector3f curInner(static_cast<float>(innerRadius*cosCur), static_cast<float>(innerRadius*sinCur), 0.0f);
    const Vector3f curOuter(static_cast<float>(outerRadius*cosCur), static_cast<float>(outerRadius*sinCur), 0.0f);

    m_Geometry.PushTri(prevInner, prevOuter, curOuter);
    m_Geometry.PushTri(curOuter, curInner, prevInner);

    prevInner = curInner;
    prevOuter = curOuter;
  }

  m_Geometry.UploadDataToBuffers();

  m_RecomputeGeometry = false;
}
