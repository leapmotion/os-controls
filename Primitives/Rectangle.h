#pragma once

#include "Primitive.h"

class Rectangle : public Primitive {
public:

  Rectangle();

  const Vector2& Size() const { return m_Size; }
  void SetSize(const Vector2& size) { m_Size = size; }

private:

  Vector2 m_Size;

};
