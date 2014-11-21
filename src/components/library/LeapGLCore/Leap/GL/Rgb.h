#pragma once

#include "Leap/GL/ColorComponent.h"

namespace Leap {
namespace GL {

// T is given as the template parameter to ColorComponent, and ColorComponent<T> is the type of
// each of the R, G, B components in this class.
// TODO: template parameter for switching the order of the components, e.g. BGR, which is a common
// ordering on some machines.
template <typename T>
class Rgb {
public:

  // TODO: probably make a component typedef.
  static const size_t COMPONENT_COUNT = 3;
  static Rgb Zero () { return Rgb(ColorComponent<T>::Zero()); }
  static Rgb One () { return Rgb(ColorComponent<T>::One()); }

  // Construct an uninitialized Rgb value.
  Rgb () { }
  // Construct an Rgb value with identical component values.
  Rgb (const ColorComponent<T> &x) : Rgb(x, x, x) { }
  // Construct an Rgb value from given components.
  Rgb (const ColorComponent<T> &r, const ColorComponent<T> &g, const ColorComponent<T> &b) {
    m_data[0] = r;
    m_data[1] = g;
    m_data[2] = b;
  }
  // // Construct an Rgb value from a templatized standard-layout type consisting of 3 T values
  // template <typename U>
  // Rgb (const U &t) {
  //   static_assert(sizeof(U) == sizeof(Rgb), "U must be a standard-layout type consisting of exactly 3 T values");
  //   static_assert(std::is_standard_layout<U>::value, "U must be a standard-layout type consisting of exactly 3 T values");
  //   // TODO: somehow check that U is actually made up of T values.
  //   memcpy(static_cast<void *>(this), static_cast<const void *>(&t), sizeof(Rgb));
  // }
  // Dynamic conversion from Rgb value with different component type.  This
  // is guaranteed to scale the dynamic range of the components appropriately.
  template <typename U>
  Rgb (const Rgb<U> &other) {
    R() = other.R();
    G() = other.G();
    B() = other.B();
  }

  bool operator == (const Rgb &other) const {
    return memcmp(static_cast<const void *>(this), static_cast<const void *>(&other), sizeof(Rgb)) == 0;
  }

  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  const U &As () const {
    static_assert(sizeof(U) == sizeof(Rgb), "U must be a POD mapping directly onto this object");
    // TODO: somehow check that U is a POD consisting only of type ColorComponent
    return *reinterpret_cast<const U *>(this);
  }
  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  U &As () {
    static_assert(sizeof(U) == sizeof(Rgb), "U must be a POD mapping directly onto this object");
    // TODO: somehow check that U is a POD consisting only of type ColorComponent
    return *reinterpret_cast<U *>(this);
  }

  // TODO: handle luminance somehow (maybe via global function)

  const ColorComponent<T> &R () const { return m_data[0]; }
  const ColorComponent<T> &G () const { return m_data[1]; }
  const ColorComponent<T> &B () const { return m_data[2]; }
  ColorComponent<T> &R () { return m_data[0]; }
  ColorComponent<T> &G () { return m_data[1]; }
  ColorComponent<T> &B () { return m_data[2]; }

  // Add another Rgb value into this one, component-wise.  Note that this may
  // overflow the component range (which is defined for each valid component by
  // ColorComponent<T>::Zero() and ColorComponent<T>::One()), and for integral component
  // types, this will also involve value wrap-around.
  void operator += (const Rgb &other) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] += other.m_data[i];
    }
  }
  Rgb operator + (const Rgb &other) const {
    Rgb retval(*this);
    retval += other;
    return retval;
  }
  void operator *= (const Rgb &other) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] *= other.m_data[i];
    }
  }
  Rgb operator * (const Rgb &other) {
    Rgb retval(*this);
    retval *= other;
    return retval;
  }
  void operator *= (const ColorComponent<T> &masking_factor) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] += masking_factor;
    }
  }
  Rgb operator * (const ColorComponent<T> &masking_factor) {
    Rgb retval(*this);
    retval *= masking_factor;
    return retval;
  }

  void Clamp () {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      ClampComponent(m_data[i]);
    }
  }
  Rgb Clamped () const {
    Rgb retval(*this);
    retval.Clamp();
    return retval;
  }

  void BlendWith (const Rgb &blend_target, const ColorComponent<T> &blend_parameter) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i].BlendWith(blend_target.m_data[i], blend_parameter);
    }
  }
  Rgb BlendedWith (const Rgb &blend_target, const ColorComponent<T> &blend_parameter) const {
    Rgb retval(*this);
    retval.BlendWith(blend_target, blend_parameter);
    return retval;
  }

private:

  ColorComponent<T> m_data[COMPONENT_COUNT];
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
