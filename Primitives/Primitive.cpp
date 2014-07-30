#include "Primitive.h"

Primitive::Primitive() {

}

Primitive::~Primitive() {

}

void Primitive::AddChild(std::shared_ptr<Primitive>& child) {
  m_Children.emplace(child);
  child->m_Parent = shared_from_this();
}

void Primitive::RemoveFromParent() {
  std::shared_ptr<Primitive> sp = m_Parent.lock();
  if (sp) {
    sp->m_Children.erase(shared_from_this());
    m_Parent.reset();
  }
}

Vector3 Primitive::LocalToGlobal(const Vector3& point) {
  // TODO: implement
  // check m_Parent
  return point;
}

Vector3 Primitive::GlobalToLocal(const Vector3& point) {
  // TODO: implement
  // check m_Parent
  return point;
}
