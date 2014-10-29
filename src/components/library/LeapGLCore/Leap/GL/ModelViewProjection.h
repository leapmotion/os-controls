#pragma once

#include "EigenTypes.h"

namespace Leap {
namespace GL {

class Projection {
public:
  Projection();
  const EigenTypes::Matrix4x4& Matrix() const;
  EigenTypes::Matrix4x4& Matrix();
  void Perspective(double left, double bottom, double right, double top, double nearClip, double farClip);
  void Perspective(double hFovRadians, double widthOverHeight, double nearClip, double farClip);
  void Orthographic(double left, double bottom, double right, double top, double nearClip, double farClip);
  EigenTypes::Vector2 Project(const EigenTypes::Vector3& point) const;
private:
  EigenTypes::Matrix4x4 m_matrix;
};

class ModelView {
public:
  ModelView();
  const EigenTypes::Matrix4x4& Matrix() const;
  EigenTypes::Matrix4x4& Matrix();
  void Reset();
  void LookAt(const EigenTypes::Vector3& eye, const EigenTypes::Vector3& center, const EigenTypes::Vector3& up);
  void Translate(const EigenTypes::Vector3& translation);
  void Rotate(const EigenTypes::Vector3& axis, double angleRadians);
  void Scale(const EigenTypes::Vector3& scale);
  void Multiply(const EigenTypes::Matrix4x4& transform);
  // This takes a Matrix3x3 M and uses it to Multiply by the block 4x4 matrix:
  //   [ M 0 ]
  //   [ 0 1 ]
  void Multiply(const EigenTypes::Matrix3x3& transform);
  void Push();
  void Pop();
  void Clear();
private:
  std::vector<EigenTypes::Matrix4x4, Eigen::aligned_allocator<EigenTypes::Matrix4x4> > m_stack;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
