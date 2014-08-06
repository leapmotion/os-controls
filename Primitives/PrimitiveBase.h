#pragma once

#include <unordered_set>
#include <memory>

#include "EigenTypes.h"
#include "ModelViewProjection.h"
#include "Color.h"

class RenderState;

// This class contains base functionality common to all primitives:
// - 3D position
// - 3D rotation, specified in Euler angles
// - Parent/child transform hierarchy (in progress)
// - Local/global coordinate system conversion (in progress)
// - Diffuse and ambient color components
// - Drawing
class PrimitiveBase : public std::enable_shared_from_this<PrimitiveBase> {

public:

  typedef std::unordered_set< std::shared_ptr<PrimitiveBase> > ChildSet;

  PrimitiveBase();
  virtual ~PrimitiveBase();

  const Vector3& Position() const { return m_Position; }
  void SetPosition(const Vector3& position) { m_Position = position; }

  void SetEulerRotation(const Vector3& rotation) { m_EulerRotation = rotation; }
  
  void SetPitch(double pitch) { m_EulerRotation.x() = pitch; }
  void SetYaw(double yaw) { m_EulerRotation.y() = yaw; }
  void SetRoll(double roll) { m_EulerRotation.z() = roll; }

  double Pitch() const { return m_EulerRotation.x(); }
  double Yaw() const { return m_EulerRotation.y(); }
  double Roll() const { return m_EulerRotation.z(); }

  void AddChild(std::shared_ptr<PrimitiveBase>& child);
  void RemoveFromParent();

  Vector3 LocalToGlobal(const Vector3& point);
  Vector3 GlobalToLocal(const Vector3& point);

  const ChildSet& Children() const { return m_Children; }
  ChildSet& Children() { return m_Children; }

  void SetDiffuseColor(const Color& color) { m_DiffuseColor = color; }
  void SetAmbientFactor(float ambient) { m_AmbientFactor = ambient; }

  virtual void Draw(RenderState& renderState) const { }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:

  void ApplyRotation(ModelView& modelView) const;

  ChildSet m_Children;
  std::weak_ptr<PrimitiveBase> m_Parent;

  // These are relative to origin, which depends on whether you have m_Parent
  Vector3 m_Position;
  Vector3 m_EulerRotation;

  Color m_DiffuseColor;
  float m_AmbientFactor;
};
