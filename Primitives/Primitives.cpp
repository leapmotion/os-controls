#include "Primitives.h"
#include "PrimitiveGeometry.h"

Sphere::Sphere() : m_Radius(1) { }

void Sphere::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitSphere(30);

  ModelView& modelView = renderState.GetModelView();

  modelView.Push();
  modelView.Translate(m_Position);
  ApplyRotation(modelView);
  modelView.Scale(Vector3::Constant(m_Radius));

  renderState.UploadMatrices();
  renderState.UploadMaterial(m_DiffuseColor, m_AmbientFactor);

  geom.Draw(renderState, GL_TRIANGLES);

  modelView.Pop();
}

Cylinder::Cylinder() : m_Radius(1), m_Height(1) { }

void Cylinder::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitCylinder(50, 1);

  ModelView& modelView = renderState.GetModelView();

  modelView.Push();
  modelView.Translate(m_Position);
  ApplyRotation(modelView);
  modelView.Scale(Vector3(m_Radius, m_Height, m_Radius));

  renderState.UploadMatrices();
  renderState.UploadMaterial(m_DiffuseColor, m_AmbientFactor);

  geom.Draw(renderState, GL_TRIANGLES);

  modelView.Pop();
}

Box::Box() : m_Size(Vector3::Constant(1.0)) { }

void Box::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitBox();

  ModelView& modelView = renderState.GetModelView();

  modelView.Push();
  modelView.Translate(m_Position);
  ApplyRotation(modelView);
  modelView.Scale(m_Size);

  renderState.UploadMatrices();
  renderState.UploadMaterial(m_DiffuseColor, m_AmbientFactor);

  geom.Draw(renderState, GL_TRIANGLES);

  modelView.Pop();
}

Disk::Disk() : m_Radius(1) { }

void Disk::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitDisk(75);

  ModelView& modelView = renderState.GetModelView();

  modelView.Push();
  modelView.Translate(m_Position);
  ApplyRotation(modelView);
  modelView.Scale(Vector3::Constant(m_Radius));

  renderState.UploadMatrices();
  renderState.UploadMaterial(m_DiffuseColor, m_AmbientFactor);

  geom.Draw(renderState, GL_TRIANGLES);

  modelView.Pop();
}

RectanglePrim::RectanglePrim() : m_Size(1, 1) { }

void RectanglePrim::Draw(RenderState& renderState) const {
  static PrimitiveGeometry geom = PrimitiveGeometry::CreateUnitSquare();

  ModelView& modelView = renderState.GetModelView();

  modelView.Push();
  modelView.Translate(m_Position);
  ApplyRotation(modelView);
  modelView.Scale(Vector3(m_Size.x(), m_Size.y(), 1.0));

  renderState.UploadMatrices();
  renderState.UploadMaterial(m_DiffuseColor, m_AmbientFactor);

  geom.Draw(renderState, GL_TRIANGLES);

  modelView.Pop();
}

