#pragma once

#include "Leap/GL/ColorComponent.h"
#include "Leap/GL/RGB.h"

namespace Leap {
namespace GL {

// TODO: template parameter for switching the order of the components, e.g. BGRA, which is a common
// ordering on some machines.
template <typename T>
class RGBA {
public:

  // TODO: probably make a component typedef.
  static const size_t COMPONENT_COUNT = 4;
  static RGBA Zero () { return RGBA(ColorComponent<T>::Zero()); }
  static RGBA One () { return RGBA(ColorComponent<T>::One()); }

  // Construct an uninitialized RGBA value.
  RGBA () { }
  // Construct an RGBA value with identical RGBA.
  RGBA (const ColorComponent<T> &x) : RGBA(x, x, x, x) { }
  // Construct an RGBA value with identical RGB components and given alpha value.
  // The default alpha value is "opaque" (max component range value).
  RGBA (const ColorComponent<T> &x, const ColorComponent<T> &a) : RGBA(x, x, x, a) { }
  // Construct an RGBA value from given components.  The default alpha value
  // is "opaque" (max component range value).
  RGBA (const ColorComponent<T> &r, const ColorComponent<T> &g, const ColorComponent<T> &b, const ColorComponent<T> &a = ColorComponent<T>::One())
    :
    m_rgb(r, g, b),
    m_alpha(a)
  { }
  // Construct an RGBA value from an RGB value and an alpha value.  The default alpha value
  // is "opaque" (max component range value).
  RGBA (const RGB<T> &rgb, const ColorComponent<T> &a = ColorComponent<T>::One())
    :
    m_rgb(rgb),
    m_alpha(a)
  { }
  // Construct an RGB value from a templatized POD type consisting of 4 ColorComponent values
  template <typename U>
  RGBA (const U &t) {
    static_assert(sizeof(U) == sizeof(RGBA), "U must be a POD consisting of exactly 4 ColorComponent values");
    // TODO: check that U is actually a POD of the required type.
    memcpy(static_cast<void *>(this), static_cast<const void *>(&t), sizeof(RGBA));
  }
  // Dynamic conversion from RGBA value with different component type.  This
  // is guaranteed to scale the dynamic range of the components losslessly.
  template <typename U>
  RGBA (const RGBA<U> &other) {
    m_rgb = other.m_rgb;
    m_alpha = other.m_alpha;
  }

  bool operator == (const RGBA &other) const {
    return memcmp(static_cast<const void *>(this), static_cast<const void *>(&other), sizeof(RGBA)) == 0;
  }

  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  const U &As () const {
    static_assert(sizeof(U) == sizeof(RGBA), "U must be a POD mapping directly onto this object");
    // TODO: somehow check that U is a POD consisting only of type ColorComponent
    return *reinterpret_cast<const U *>(this);
  }
  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  U &As () {
    static_assert(sizeof(U) == sizeof(RGBA), "U must be a POD mapping directly onto this object");
    // TODO: somehow check that U is a POD consisting only of type ColorComponent
    return *reinterpret_cast<U *>(this);
  }

  // TODO: handle luminance somehow (maybe via global function)

  const ColorComponent<T> &R () const { return m_rgb.R(); }
  const ColorComponent<T> &G () const { return m_rgb.G(); }
  const ColorComponent<T> &B () const { return m_rgb.B(); }
  const ColorComponent<T> &A () const { return m_alpha; }
  ColorComponent<T> &R () { return m_rgb.R(); }
  ColorComponent<T> &G () { return m_rgb.G(); }
  ColorComponent<T> &B () { return m_rgb.B(); }
  ColorComponent<T> &A () { return m_alpha; }

  const RGB<T> &Rgb () const { return m_rgb; }
  RGB<T> &Rgb () { return m_rgb; }

  // Add another RGBA value into this one, component-wise.  Note that this may
  // overflow the component range (which is defined for each valid component by
  // ColorComponent<T>::Zero() and ColorComponent<T>::One()), and for integral component
  // types, this will also involve value wrap-around.
  void operator += (const RGBA &other) {
    m_rgb += other.m_rgb;
    m_alpha += other.m_alpha;
  }
  RGBA operator + (const RGBA &other) const {
    RGBA retval(*this);
    retval += other;
    return retval;
  }
  void operator *= (const RGBA &other) {
    m_rgb *= other.m_rgb;
    m_alpha *= other.m_alpha;
  }
  RGBA operator * (const RGBA &other) {
    RGBA retval(*this);
    retval *= other;
    return retval;
  }
  void operator *= (const ColorComponent<T> &masking_factor) {
    m_rgb *= masking_factor;
    m_alpha *= masking_factor;
  }
  RGBA operator * (const ColorComponent<T> &masking_factor) {
    RGBA retval(*this);
    retval *= masking_factor;
    return retval;
  }

  void Clamp () {
    m_rgb.Clamp();
    m_alpha.Clamp();
  }
  RGBA Clamped () const {
    RGBA retval(*this);
    retval.Clamp();
    return retval;
  }

  void BlendWith (const RGBA &blend_target, const ColorComponent<T> &blend_parameter) {
    m_rgb.BlendWith(blend_target.m_rgb, blend_parameter);
  }
  RGBA BlendedWith (const RGBA &blend_target, const ColorComponent<T> &blend_parameter) const {
    RGBA retval(*this);
    retval.BlendWith(blend_target, blend_parameter);
    return retval;
  }

  void PremultiplyAlpha () {
    m_rgb *= m_alpha;
  }

private:

  RGB<T> m_rgb;
  ColorComponent<T> m_alpha;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
