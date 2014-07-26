/*==================================================================================================================

    Copyright (c) 2010 - 2013 Leap Motion. All rights reserved.

  The intellectual and technical concepts contained herein are proprietary and confidential to Leap Motion, and are
  protected by trade secret or copyright law. Dissemination of this information or reproduction of this material is
  strictly forbidden unless prior written permission is obtained from Leap Motion.

===================================================================================================================*/

/// <summary>
/// Primitives for Figure Display
/// </summary>
/// <remarks>
///
/// Maintainers: Jimmy
/// </remarks>

#ifndef __FigurePrimitive_h__
#define __FigurePrimitive_h__

#include "FigureColor.h"
#include "DataTypes.h"
#include "GLBuffer.h"
#include "UtilGL.h"

struct RenderInfo {
  RenderInfo(int posAddr_, int normalAddr_, int modelViewAddr_, int projAddr_, int normalMatrixAddr_, int colorAddr_, ModelView& modelView_, Projection& projection_)
    : posAddr(posAddr_), normalAddr(normalAddr_), modelViewAddr(modelViewAddr_), projAddr(projAddr_), normalMatrixAddr(normalMatrixAddr_), colorAddr(colorAddr_), modelView(modelView_), projection(projection_)
  { }

  // attributes
  int posAddr;
  int normalAddr;

  // uniforms
  int modelViewAddr;
  int projAddr;
  int normalMatrixAddr;
  int colorAddr;

  // transformations
  ModelView& modelView;
  Projection& projection;
};

class FigurePrimitive {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  FigurePrimitive(const FigureColor& color_) : color(color_) {}
  virtual void Render(RenderInfo& info) const = 0;
  const FigureColor& Color() const { return color; }
  virtual ~FigurePrimitive() {}
protected:
  FigureColor color;
};

class FigureSphere : public FigurePrimitive
{
public:
  FigureSphere(const Vector3& p_, double radius_, const FigureColor& color_ = FigureColor::White()) :
    p(p_), radius(radius_), FigurePrimitive(color_) {}
  void Render(RenderInfo& info) const override;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Vector3 p;
  double radius;
};

class FigureEllipsoid : public FigurePrimitive
{
public:
  FigureEllipsoid(const Vector3& p_, const Matrix3x3& axes_, const FigureColor& color_ = FigureColor::White()) :
    p(p_), axes(axes_), FigurePrimitive(color_) {}
  void Render(RenderInfo& info) const override;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Vector3 p;
  Matrix3x3 axes;
};

class FigureCylinder : public FigurePrimitive
{
public:
  FigureCylinder(const Vector3& p0_, const Vector3& p1_, double radius_, const FigureColor& color_ = FigureColor::White()) :
    p0(p0_), p1(p1_), radius(radius_), FigurePrimitive(color_) {}
  void Render(RenderInfo& info) const override;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Vector3 p0;
  Vector3 p1;
  double radius;
};

class FigureBox : public FigurePrimitive
{
public:
  FigureBox(const Vector3& p_, const Matrix3x3& axes_, const FigureColor& color_ = FigureColor::White()) :
    p(p_), axes(axes_), FigurePrimitive(color_) {}
  void Render(RenderInfo& info) const override;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Vector3 p;
  Matrix3x3 axes;
};

#endif
