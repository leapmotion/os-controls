// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "Geometry.h"

namespace leap {

class ScreenBase {
  public:
    ScreenBase() : m_bounds(RectZero) {}
    virtual ~ScreenBase() {}

    inline Rect Bounds() const { return m_bounds; }
    inline Point Origin() const { return m_bounds.origin; }
    inline Size Size() const { return m_bounds.size; }
    inline Float X() const { return m_bounds.origin.x; }
    inline Float Y() const { return m_bounds.origin.y; }
    inline Float Width() const { return m_bounds.size.width; }
    inline Float Height() const { return m_bounds.size.height; }

    inline Point ClipPosition(const Point& position) const {
      const Rect& rect = m_bounds;
      const Float minX = RectGetMinX(rect);
      const Float minY = RectGetMinY(rect);
      const Float maxX = RectGetMaxX(rect);
      const Float maxY = RectGetMaxY(rect);
      Float x = position.x;
      Float y = position.y;
      if (x <= minX) { x = minX; } else if (x >= maxX) { x = maxX - 1; }
      if (y <= minY) { y = minY; } else if (y >= maxY) { y = maxY - 1; }
      return PointMake(x, y);
    }

    inline Point Normalize(const Point& position) const {
      const auto& origin = m_bounds.origin;
      const auto& size = m_bounds.size;
      if (size.width > 0 && size.height > 0) {
        return PointMake((position.x - origin.x)/size.width, (position.y - origin.y)/size.height);
      }
      return PointZero;
    }

    inline Point Denormalize(const Point& position) const {
      const auto& origin = m_bounds.origin;
      const auto& size = m_bounds.size;

      return PointMake(position.x*size.width + origin.x, position.y*size.height + origin.y);
    }

    inline Float AspectRatio() const {
      if (Height() < 1) {
        return static_cast<Float>(1);
      }
      return Width()/Height();
    }

  protected:
    Rect m_bounds;
};

}
