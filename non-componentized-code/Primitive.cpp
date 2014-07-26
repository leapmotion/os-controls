/*==================================================================================================================

    Copyright (c) 2010 - 2013 Leap Motion. All rights reserved.

  The intellectual and technical concepts contained herein are Proprietary and confidential to Leap Motion, and are
  protected by trade secret or copyright law. Dissemination of this information or reproduction of this material is
  strictly forbidden unless prior written permission is obtained from Leap Motion.

===================================================================================================================*/
#include "FigurePrimitive.h"
#include "Utility/MathUtil.h"
#include "Visualization/GLIncludes.h"
#include "UtilGL.h"

#include <map>

#if _WIN32
#include "SDL.h"
#include "SDL_opengl.h"
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#endif
#undef main

#define NUM_CYLINDER_SIDES 10

void FigureSphere::Render(RenderInfo& info) const {
  info.modelView.Push();
  info.modelView.Translate(p);
  info.modelView.Scale(Vector3::Constant(radius));
  info.projection.SetUniform(info.projAddr);
  info.modelView.SetUniform(info.modelViewAddr);
  UtilGL::NormalMatrixToUniform(info.projection, info.modelView, info.normalMatrixAddr);
  UtilGL::Vector4ToUniform(color.Data().cast<double>(), info.colorAddr);
  UtilGL::DrawUnitSphere(info.posAddr, info.normalAddr);
  info.modelView.Pop();
}

void FigureEllipsoid::Render(RenderInfo& info) const {
  info.modelView.Push();
  info.modelView.Translate(p);
  Matrix4x4 mat(Matrix4x4::Identity());
  mat.block<3, 3>(0, 0) << axes;
  info.modelView.Multiply(mat);
  info.projection.SetUniform(info.projAddr);
  info.modelView.SetUniform(info.modelViewAddr);
  UtilGL::NormalMatrixToUniform(info.projection, info.modelView, info.normalMatrixAddr);
  UtilGL::Vector4ToUniform(color.Data().cast<double>(), info.colorAddr);
  UtilGL::DrawUnitSphere(info.posAddr, info.normalAddr);
  info.modelView.Pop();
}

void FigureCylinder::Render(RenderInfo& info) const {
  info.modelView.Push();
  info.modelView.Translate(p0);
  Vector3 direction = p1 - p0;
  const double length = direction.norm();
  direction /= length;

  Vector3 Y = direction;
  Vector3 X = Y.cross(Vector3::UnitZ()).normalized();
  Vector3 Z = Y.cross(X).normalized();
  Matrix4x4 basis(Matrix4x4::Identity());
  basis.block<3, 3>(0, 0) << X, Y, Z;
  info.modelView.Multiply(basis);

  info.modelView.Scale(Vector3(radius, length, radius));
  info.projection.SetUniform(info.projAddr);
  info.modelView.SetUniform(info.modelViewAddr);
  UtilGL::NormalMatrixToUniform(info.projection, info.modelView, info.normalMatrixAddr);
  UtilGL::Vector4ToUniform(color.Data().cast<double>(), info.colorAddr);
  UtilGL::DrawUnitCylinder(info.posAddr, info.normalAddr);
  info.modelView.Pop();
}

void FigureBox::Render(RenderInfo& info) const {
  info.modelView.Push();
  info.modelView.Translate(p);
  Matrix4x4 mat(Matrix4x4::Identity());
  mat.block<3, 3>(0, 0) << axes;
  info.modelView.Multiply(mat);
  info.projection.SetUniform(info.projAddr);
  info.modelView.SetUniform(info.modelViewAddr);
  UtilGL::NormalMatrixToUniform(info.projection, info.modelView, info.normalMatrixAddr);
  UtilGL::Vector4ToUniform(color.Data().cast<double>(), info.colorAddr);
  UtilGL::DrawUnitBox(info.posAddr, info.normalAddr);
  info.modelView.Pop();
}
