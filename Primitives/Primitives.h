#pragma once

#include "PrimitiveBase.h"
#include "RenderState.h"

class Sphere : public PrimitiveBase {
public:

  Sphere();
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  virtual void Draw(RenderState& renderState) const;

private:
  double m_Radius;
};

class Ellipsoid : public PrimitiveBase {

};

class Cylinder : public PrimitiveBase {
public:

  Cylinder();
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double Height() const { return m_Height; }
  void SetHeight(double height) { m_Height = height; }

  virtual void Draw(RenderState& renderState) const;

private:
  double m_Radius;
  double m_Height;
};

class Box : public PrimitiveBase {
public:

  Box();
  const Vector3& Size() const { return m_Size; }
  void SetSize(const Vector3& size) { m_Size = size; }

  virtual void Draw(RenderState& renderState) const;

private:
  Vector3 m_Size;
};

class Disk : public PrimitiveBase {
public:

  Disk();
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  virtual void Draw(RenderState& renderState) const;

private:
  double m_Radius;
};

class Ellipse : public PrimitiveBase {

};

class RectanglePrim : public PrimitiveBase {
public:

  RectanglePrim();
  const Vector2& Size() const { return m_Size; }
  void SetSize(const Vector2& size) { m_Size = size; }

  virtual void Draw(RenderState& renderState) const;

private:

  Vector2 m_Size;
};
