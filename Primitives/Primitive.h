#pragma once

#include <unordered_set>
#include <memory>
#include "EigenTypes.h"

class Primitive : public std::enable_shared_from_this<Primitive> {

public:

  typedef std::unordered_set< std::shared_ptr<Primitive> > ChildSet;

  Primitive();
  virtual ~Primitive();

  const Vector3& Position() const { return m_Position; }
  void SetPosition(const Vector3& position) { m_Position = position; }

  void SetEulerRotation(const Vector3& rotation) { m_EulerRotation = rotation; }
  
  void SetPitch(double pitch) { m_EulerRotation.x() = pitch; }
  void SetYaw(double yaw) { m_EulerRotation.y() = yaw; }
  void SetRoll(double roll) { m_EulerRotation.z() = roll; }

  double Pitch() const { return m_EulerRotation.x(); }
  double Yaw() const { return m_EulerRotation.y(); }
  double Roll() const { return m_EulerRotation.z(); }

  void AddChild(std::shared_ptr<Primitive>& child);
  void RemoveFromParent();

  Vector3 LocalToGlobal(const Vector3& point);
  Vector3 GlobalToLocal(const Vector3& point);

  const ChildSet& Children() const { return m_Children; }
  ChildSet& Children() { return m_Children; }

private:

  ChildSet m_Children;
  std::weak_ptr<Primitive> m_Parent;

  // These are relative to origin, which depends on whether you have m_Parent
  Vector3 m_Position;
  Vector3 m_EulerRotation;

};
