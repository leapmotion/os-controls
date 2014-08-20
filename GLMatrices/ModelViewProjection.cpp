#include "ModelViewProjection.h"

Projection::Projection() : m_matrix(Matrix4x4::Identity()) { }

const Matrix4x4& Projection::Matrix() const {
  return m_matrix;
}

void Projection::Perspective(double left, double bottom, double right, double top, double nearClip, double farClip) {
  const double denom = 1/(nearClip - farClip);
  m_matrix << 2/(right - left),                0, (right + left)/(right - left),                        0,
                             0, 2/(top - bottom), (top + bottom)/(top - bottom),                        0,
                             0,                0,    (farClip + nearClip)*denom, 2*farClip*nearClip*denom,
                             0,                0,                            -1,                        0;
}

void Projection::Perspective(double hFovRadians, double widthOverHeight, double nearClip, double farClip) {
  const double wd2 = nearClip * std::tan(hFovRadians / 2.0);
  const double left = -wd2;
  const double right = wd2;
  const double top = wd2/widthOverHeight;
  const double bottom = -wd2/widthOverHeight;
  Perspective(left, bottom, right, top, nearClip, farClip);
}

void Projection::Orthographic(double left, double bottom, double right, double top, double nearClip, double farClip) {
  m_matrix << 2/(right - left),                0,                      0,             (right + left)/(left - right),
                             0, 2/(top - bottom),                      0,             (top + bottom)/(bottom - top),
                             0,                0, 2/(farClip - nearClip), (farClip + nearClip)/(farClip - nearClip),
                             0,                0,                      0,                                         1;
}

Vector2 Projection::Project(const Vector3& point) const {
  Vector2 result = (m_matrix * Vector4(point.x(), point.y(), point.z(), 1.0)).head<2>();
  result.x() = (result.x() + 1)/2;
  result.y() = (result.y() + 1)/2;
  return result;
}

ModelView::ModelView() {
  m_stack.push_back(Matrix4x4::Identity());
}

const Matrix4x4& ModelView::Matrix() const {
  return m_stack.back();
}

void ModelView::Reset() {
  m_stack.back().setIdentity();
}

void ModelView::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
  Matrix4x4& mat = m_stack.back();
  Vector3 z = (eye - center).normalized();
  Vector3 y = up;
  Vector3 x = y.cross(z).normalized();
  y = z.cross(x).normalized();
  mat.setIdentity();
  mat.col(0) << x, 0;
  mat.col(1) << y, 0;
  mat.col(2) << z, 0;
  mat.col(3) << -x.dot(eye), -y.dot(eye), -z.dot(eye), 1.0;
}

void ModelView::Translate(const Vector3& translation) {
  // TODO: replace with some utility function
  Matrix4x4 mat = Matrix4x4::Identity();
  mat(0, 3) = translation[0];
  mat(1, 3) = translation[1];
  mat(2, 3) = translation[2];
  m_stack.back() *= mat;
}

void ModelView::Rotate(const Vector3& axis, double angleRadians) {
  // TODO: replace with some utility function
  Matrix4x4 mat;
  const double c = std::cos(angleRadians);
  const double s = std::sin(angleRadians);
  const double C = (1 - c);
  mat << axis[0] * axis[0] * C + c, axis[0] * axis[1] * C - axis[2] * s, axis[0] * axis[2] * C + axis[1] * s, 0,
         axis[1] * axis[0] * C + axis[2] * s, axis[1] * axis[1] * C + c, axis[1] * axis[2] * C - axis[0] * s, 0,
         axis[2] * axis[0] * C - axis[1] * s, axis[2] * axis[1] * C + axis[0] * s, axis[2] * axis[2] * C + c, 0,
                                           0,                                   0,                         0, 1;
  m_stack.back() *= mat;
}

void ModelView::Scale(const Vector3& scale) {
  // TODO: replace with some utility function
  Matrix4x4 mat = Matrix4x4::Identity();
  mat(0, 0) = scale[0];
  mat(1, 1) = scale[1];
  mat(2, 2) = scale[2];
  m_stack.back() *= mat;
}

void ModelView::Multiply(const Matrix4x4& transform) {
  m_stack.back() *= transform;
}

void ModelView::Multiply(const Matrix3x3& transform) {
  Matrix4x4 affine;
  affine.block<3,3>(0,0) = transform;
  affine.block<3,1>(0,3).setZero();
  affine.block<1,3>(3,0).setZero();
  affine(3,3) = MATH_TYPE(1);
  Multiply(affine);
}

void ModelView::Push() {
  m_stack.push_back(m_stack.back());
}

void ModelView::Pop() {
  assert(m_stack.size() > 1);
  m_stack.pop_back();
}
