/*==================================================================================================================

    Copyright (c) 2010 - 2013 Leap Motion. All rights reserved.

  The intellectual and technical concepts contained herein are Proprietary and confidential to Leap Motion, and are
  protected by trade secret or copyright law. Dissemination of this information or reproduction of this material is
  strictly forbidden unless prior written permission is obtained from Leap Motion.

===================================================================================================================*/
#include "FigureColor.h"
#include "Utility/MathUtil.h"

void FigureColor::FromHSV(float h, float s, float v, float a) {
  float hh, p, q, t, ff;
  long i;
  if (s <= 0.0f) {
    m_data << 0.0f, 0.0f, 0.0f, a;
    return;
  }
  hh = h;
  if (hh >= 360.0f) {
    hh = 0.0f;
  }
  hh /= 60.0f;
  i = (long)hh;
  ff = hh - i;
  p = v * (1.0f - s);
  q = v * (1.0f - (s * ff));
  t = v * (1.0f - (s * (1.0f - ff)));
  switch (i) {
  case 0: m_data << v, t, p, a; break;
  case 1: m_data << q, v, p, a; break;
  case 2: m_data << p, v, t, a; break;
  case 3: m_data << p, q, v, a; break;
  case 4: m_data << t, p, v, a; break;
  case 5:
  default: m_data << v, p, q, a; break;
  }
}

void FigureColor::ToHSV(float& h, float& s, float& v) const {
  const float r = R();
  const float g = G();
  const float b = B();
  float min, max, delta;
  min = r < g ? r : g;
  min = min < b ? min : b;
  max = r > g ? r : g;
  max = max > b ? max : b;
  v = max;
  delta = max - min;
  if (max > OCU_EPSILON) {
    s = (delta / max);
  } else {
    s = 0.0f;
    h = 0;
    return;
  }
  if (r >= max) {
    h = (g - b) / delta;
  } else if (g >= max) {
    h = 2.0f + (b - r) / delta;
  } else {
    h = 4.0f + (r - g) / delta;
  }
  h *= 60.0f;
  if (h < 0.0f) {
    h += 360.0f;
  }
}
