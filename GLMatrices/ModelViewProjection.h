#pragma once

#include "gl_glext_glu.h"
#include "EigenTypes.h"

class Projection {
public:
  Projection();
  const Matrix4x4& Matrix() const;
  void LoadFromCurrent();
  void SetCurrent();
  void Perspective(double left, double bottom, double right, double top, double nearClip, double farClip);
  void Perspective(double hFovRadians, double widthOverHeight, double nearClip, double farClip);
  void Orthographic(double left, double bottom, double right, double top, double nearClip, double farClip);
  Vector2 Project(const Vector3& point) const;
  void SetUniform(int address) const;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Matrix4x4 m_matrix;
};

class ModelView {
public:
  ModelView();
  const Matrix4x4& Matrix() const;
  void LoadFromCurrent();
  void SetCurrent();
  void Reset();
  void LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
  void Translate(const Vector3& translation);
  void Rotate(const Vector3& axis, double angleRadians);
  void Scale(const Vector3& scale);
  void Multiply(const Matrix4x4& transform);
  // This takes a Matrix3x3 M and uses it to Multiply by the block 4x4 matrix:
  //   [ M 0 ]
  //   [ 0 1 ]
  void Multiply(const Matrix3x3& transform);
  void SetUniform(int address) const;
  void Push();
  void Pop();
private:
  std::vector<Matrix4x4, Eigen::aligned_allocator<Matrix4x4> > m_stack;
};
