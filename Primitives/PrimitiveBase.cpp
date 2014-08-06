#include "PrimitiveBase.h"

PrimitiveBase::PrimitiveBase() :
  m_Position(Vector3::Zero()),
  m_EulerRotation(Vector3::Zero()),
  m_DiffuseColor(Color::White()),
  m_AmbientFactor(0.0f)
{

}

PrimitiveBase::~PrimitiveBase() {

}

void PrimitiveBase::AddChild(std::shared_ptr<PrimitiveBase>& child) {
  m_Children.emplace(child);
  child->m_Parent = shared_from_this();
}

void PrimitiveBase::RemoveFromParent() {
  std::shared_ptr<PrimitiveBase> sp = m_Parent.lock();
  if (sp) {
    sp->m_Children.erase(shared_from_this());
    m_Parent.reset();
  }
}

Vector3 PrimitiveBase::LocalToGlobal(const Vector3& point) {
  // TODO: implement
  // check m_Parent
  return point;
}

Vector3 PrimitiveBase::GlobalToLocal(const Vector3& point) {
  // TODO: implement
  // check m_Parent
  return point;
}

void PrimitiveBase::ApplyRotation(ModelView& modelView) const {
  modelView.Rotate(Vector3::UnitX(), Pitch());
  modelView.Rotate(Vector3::UnitY(), Yaw());
  modelView.Rotate(Vector3::UnitZ(), Roll());
}
