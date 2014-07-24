/*==================================================================================================================

    Copyright (c) 2010 - 2013 Leap Motion. All rights reserved.

  The intellectual and technical concepts contained herein are proprietary and confidential to Leap Motion, and are
  protected by trade secret or copyright law. Dissemination of this information or reproduction of this material is
  strictly forbidden unless prior written permission is obtained from Leap Motion.

===================================================================================================================*/

/// <summary>
/// Colors for internal visualizer
/// </summary>
/// <remarks>
///
/// Maintainers: Raffi, Jimmy
/// </remarks>

#include "DataTypes.h"

#ifndef __FigureColor_h__
#define __FigureColor_h__

class FigureColor {
public:
  FigureColor() {
    m_data << 1.0f, 1.0f, 1.0f, 1.0f;
  }
  //TODO: make this constructor not stamp on the other one somehow
  FigureColor(float r, float g, float b, float a = 1.0f) {
    m_data << r, g, b, a;
  }
  FigureColor(const Vector3& c) {
    m_data << static_cast<float>(c.x()), static_cast<float>(c.y()), static_cast<float>(c.z()), 1.0f;
  }
  FigureColor(const Vector4& c) {
    m_data << static_cast<float>(c.x()), static_cast<float>(c.y()), static_cast<float>(c.z()), static_cast<float>(c.w());
  }
  FigureColor(const FigureColor& other, float blend) {
    m_data << other.R(), other.G(), other.B(), blend*other.A();
  }
  FigureColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    m_data << r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f;
  }
  FigureColor(int id, float a = 1.0f) {
    m_data << static_cast<float>((id * 17 + 50) % 100) / 125.f + 0.2f, static_cast<float>((id * 31) % 100) / 125.f + 0.2f, static_cast<float>((id * 71) % 100) / 125.f + 0.2f, a;
  }

  // convert to single brightness based on human perception
  float Luminance() const { return 0.2126f*R() + 0.7152f*G() + 0.0722f*B(); }

  // simple getters
  float R() const { return m_data[0]; }
  float G() const { return m_data[1]; }
  float B() const { return m_data[2]; }
  float A() const { return m_data[3]; }
  float& R() { return m_data[0]; }
  float& G() { return m_data[1]; }
  float& B() { return m_data[2]; }
  float& A() { return m_data[3]; }
  const Eigen::Matrix<float, 4, 1>& Data() const { return m_data; }

  // HSV conversions
  void FromHSV(float h, float s, float v, float a = 1.0f);
  void ToHSV(float& h, float& s, float& v) const;

  // common colors
  static const FigureColor& White() { static FigureColor c(1.0f, 1.0f, 1.0f, 1.0f); return c; }
  static const FigureColor& LightGrey() { static FigureColor c(0.85f, 0.85f, 0.85f, 1.0f); return c; }
  static const FigureColor& Grey() { static FigureColor c(0.6f, 0.6f, 0.6f, 1.0f); return c; }
  static const FigureColor& DarkGrey() { static FigureColor c(0.3f, 0.3f, 0.3f, 1.0f); return c; }
  static const FigureColor& Black() { static FigureColor c(0.0f, 0.0f, 0.0f, 1.0f); return c; }
  static const FigureColor& Red() { static FigureColor c(1.0f, 0.0f, 0.0f, 1.0f); return c; }
  static const FigureColor& Blue() { static FigureColor c(0.0f, 0.0f, 1.0f, 1.0f); return c; }
  static const FigureColor& Green() { static FigureColor c(0.0f, 1.0f, 0.0f, 1.0f); return c; }
  static const FigureColor& Yellow() { static FigureColor c(1.0f, 1.0f, 0.0f, 1.0f); return c; }
  static const FigureColor& Purple() { static FigureColor c(1.0f, 0.0f, 1.0f, 1.0f); return c; }
  static const FigureColor& Orange() { static FigureColor c(1.0f, 0.6f, 0.0f, 1.0f); return c; }
  static const FigureColor& Teal() { static FigureColor c(0.0f, 1.0f, 1.0f, 1.0f); return c; }
  static const FigureColor& Transparent() { static FigureColor c(0.0f, 0.0f, 0.0f, 0.0f); return c; }

private:
  Eigen::Matrix<float, 4, 1> m_data;
};

#endif
