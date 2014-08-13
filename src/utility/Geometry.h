// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
#if _WIN32
  #undef min //stupid windows.h...
  #undef max
#endif
#include <algorithm>
#endif

namespace leap {

typedef float Float;

#if __APPLE__
using Point = CGPoint;
using Size = CGSize;
using Rect = CGRect;
static const Point& PointZero = CGPointZero;
static const Size& SizeZero = CGSizeZero;
static const Rect& RectZero = CGRectZero;
static inline Point PointMake(Float x, Float y) { return CGPointMake(x, y); }
static inline Size SizeMake(Float width, Float height) { return CGSizeMake(width, height); }
static inline Rect RectMake(Float x, Float y, Float width, Float height) { return CGRectMake(x, y, width, height); }
static inline Float RectGetMinX(const Rect& r) { return CGRectGetMinX(r); }
static inline Float RectGetMinY(const Rect& r) { return CGRectGetMinY(r); }
static inline Float RectGetMaxX(const Rect& r) { return CGRectGetMaxX(r); }
static inline Float RectGetMaxY(const Rect& r) { return CGRectGetMaxY(r); }
static inline bool RectContainsPoint(const Rect& r, const Point& p) { return CGRectContainsPoint(r, p); }
static inline Rect RectUnion(const Rect& r1, const Rect& r2) { return CGRectUnion(r1, r2); }
#else
struct Point {
  Point(Float _x = 0, Float _y = 0) : x(_x), y(_y) {}
  Float x;
  Float y;
};

struct Size {
  Size(Float _width = 0, Float _height = 0) : width(_width), height(_height) {}
  Float width;
  Float height;
};

struct Rect {
  Rect(Float _x = 0, Float _y = 0, Float _width = 0, Float _height = 0) : origin(_x, _y), size(_width, _height) {}
  Point origin;
  Size size;
};

static Point PointZero;
static inline Point PointMake(Float x, Float y) { return Point(x, y); }
static Size SizeZero;
static inline Size SizeMake(Float width, Float height) { return Size(width, height); }
static Rect RectZero;
static inline Rect RectMake(Float x, Float y, Float width, Float height) {
  return Rect(x, y, width, height);
}

static inline Float RectGetMinX(const Rect& r) { return r.origin.x; }
static inline Float RectGetMinY(const Rect& r) { return r.origin.y; }
static inline Float RectGetMaxX(const Rect& r) { return (r.origin.x + r.size.width); }
static inline Float RectGetMaxY(const Rect& r) { return (r.origin.y + r.size.height); }

static inline bool RectContainsPoint(const Rect& r, const Point& p) {
  return (p.x >= r.origin.x && p.x < (r.origin.x + r.size.width) &&
          p.y >= r.origin.y && p.y < (r.origin.y + r.size.height));
}

static inline Rect RectUnion(const Rect& r1, const Rect& r2)
{
  Float x0 = std::min(r1.origin.x, r2.origin.x);
  Float x1 = std::max(r1.origin.x + r1.size.width,
                        r2.origin.x + r2.size.width);
  Float y0 = std::min(r1.origin.y, r2.origin.y);
  Float y1 = std::max(r1.origin.y + r1.size.height,
                        r2.origin.y + r2.size.height);
  return Rect(x0, y0, x1 - x0, y1 - y0);
}
#endif

#if !_WIN32
typedef struct _RECT {
  int32_t left;
  int32_t right;
  int32_t top;
  int32_t bottom;
} RECT, *PRECT;

typedef struct _POINT {
  int32_t x;
  int32_t y;
} POINT, *PPOINT;

typedef struct _SIZE {
  int32_t cx;
  int32_t cy;
} SIZE, *PSIZE;
#endif

}
