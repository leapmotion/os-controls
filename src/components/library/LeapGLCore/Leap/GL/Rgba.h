#pragma once

#include "Leap/GL/ColorComponent.h"
#include "Leap/GL/Rgb.h"

namespace Leap {
namespace GL {

// TODO: template parameter for switching the order of the components, e.g. BGRA, which is a common
// ordering on some machines.
template <typename T>
class Rgba {
public:

  // TODO: probably make a component typedef.
  static const size_t COMPONENT_COUNT = 4;
  static Rgba Zero () { return Rgba(ColorComponent<T>::Zero()); }
  static Rgba One () { return Rgba(ColorComponent<T>::One()); }

  // Construct an uninitialized Rgba value.
  Rgba () { }
  // Construct an Rgba value with identical Rgba.
  Rgba (const ColorComponent<T> &x) : Rgba(x, x, x, x) { }
  // Construct an Rgba value with identical Rgb components and given alpha value.
  // The default alpha value is "opaque" (max component range value).
  Rgba (const ColorComponent<T> &x, const ColorComponent<T> &a) : Rgba(x, x, x, a) { }
  // Construct an Rgba value from given components.  The default alpha value
  // is "opaque" (max component range value).
  Rgba (const ColorComponent<T> &r, const ColorComponent<T> &g, const ColorComponent<T> &b, const ColorComponent<T> &a = ColorComponent<T>::One())
    :
    m_rgb(r, g, b),
    m_alpha(a)
  { }
  // Construct an Rgba value from an Rgb value and an alpha value.  The default alpha value
  // is "opaque" (max component range value).
  Rgba (const Rgb<T> &rgb, const ColorComponent<T> &a = ColorComponent<T>::One())
    :
    m_rgb(rgb),
    m_alpha(a)
  { }
  // Construct an Rgb value from a templatized POD type consisting of 4 ColorComponent values
  template <typename U>
  Rgba (const U &t) {
    static_assert(sizeof(U) == sizeof(Rgba), "U must be a POD consisting of exactly 4 ColorComponent values");
    // TODO: check that U is actually a POD of the required type.
    memcpy(static_cast<void *>(this), static_cast<const void *>(&t), sizeof(Rgba));
  }
  // Dynamic conversion from Rgba value with different component type.  This
  // is guaranteed to scale the dynamic range of the components losslessly.
  template <typename U>
  Rgba (const Rgba<U> &other) {
    m_rgb = other.m_rgb;
    m_alpha = other.m_alpha;
  }

  bool operator == (const Rgba &other) const {
    return memcmp(static_cast<const void *>(this), static_cast<const void *>(&other), sizeof(Rgba)) == 0;
  }

  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  const U &As () const {
    static_assert(sizeof(U) == sizeof(Rgba), "U must be a POD mapping directly onto this object");
    // TODO: somehow check that U is a POD consisting only of type ColorComponent
    return *reinterpret_cast<const U *>(this);
  }
  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  U &As () {
    static_assert(sizeof(U) == sizeof(Rgba), "U must be a POD mapping directly onto this object");
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

  const Leap::GL::Rgb<T> &Rgb () const { return m_rgb; }
  Leap::GL::Rgb<T> &Rgb () { return m_rgb; }

  // Add another Rgba value into this one, component-wise.  Note that this may
  // overflow the component range (which is defined for each valid component by
  // ColorComponent<T>::Zero() and ColorComponent<T>::One()), and for integral component
  // types, this will also involve value wrap-around.
  void operator += (const Rgba &other) {
    m_rgb += other.m_rgb;
    m_alpha += other.m_alpha;
  }
  Rgba operator + (const Rgba &other) const {
    Rgba retval(*this);
    retval += other;
    return retval;
  }
  void operator *= (const Rgba &other) {
    m_rgb *= other.m_rgb;
    m_alpha *= other.m_alpha;
  }
  Rgba operator * (const Rgba &other) {
    Rgba retval(*this);
    retval *= other;
    return retval;
  }
  void operator *= (const ColorComponent<T> &masking_factor) {
    m_rgb *= masking_factor;
    m_alpha *= masking_factor;
  }
  Rgba operator * (const ColorComponent<T> &masking_factor) {
    Rgba retval(*this);
    retval *= masking_factor;
    return retval;
  }

  void Clamp () {
    m_rgb.Clamp();
    m_alpha.Clamp();
  }
  Rgba Clamped () const {
    Rgba retval(*this);
    retval.Clamp();
    return retval;
  }

  void BlendWith (const Rgba &blend_target, const ColorComponent<T> &blend_parameter) {
    m_rgb.BlendWith(blend_target.m_rgb, blend_parameter);
  }
  Rgba BlendedWith (const Rgba &blend_target, const ColorComponent<T> &blend_parameter) const {
    Rgba retval(*this);
    retval.BlendWith(blend_target, blend_parameter);
    return retval;
  }

  void PremultiplyAlpha () {
    m_rgb *= m_alpha;
  }

private:

  Leap::GL::Rgb<T> m_rgb;
  ColorComponent<T> m_alpha;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
