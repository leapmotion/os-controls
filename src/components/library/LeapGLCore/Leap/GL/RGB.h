#pragma once

#include "ColorComponent.h"

// T is given as the template parameter to ColorComponent, and ColorComponent<T> is the type of
// each of the R, G, B components in this class.
// TODO: template parameter for switching the order of the components, e.g. BGR, which is a common
// ordering on some machines.
template <typename T>
class RGB {
public:

  // TODO: probably make a component typedef.
  static const size_t COMPONENT_COUNT = 3;
  static RGB Zero () { return RGB(ColorComponent<T>::Zero()); }
  static RGB One () { return RGB(ColorComponent<T>::One()); }

  // Construct an uninitialized RGB value.
  RGB () { }
  // Construct an RGB value with identical component values.
  RGB (const ColorComponent<T> &x) : RGB(x, x, x) { }
  // Construct an RGB value from given components.
  RGB (const ColorComponent<T> &r, const ColorComponent<T> &g, const ColorComponent<T> &b) {
    m_data[0] = r;
    m_data[1] = g;
    m_data[2] = b;
  }
  // Construct an RGB value from a templatized POD type consisting of 3 T values
  template <typename U>
  RGB (const U &t) {
    static_assert(sizeof(U) == sizeof(RGB), "U must be a POD consisting of exactly 3 T values");
    // TODO: check that U is actually a POD of the required type.
    memcpy(static_cast<void *>(this), static_cast<const void *>(&t), sizeof(RGB));
  }
  // Dynamic conversion from RGB value with different component type.  This
  // is guaranteed to scale the dynamic range of the components losslessly.
  template <typename U>
  RGB (const RGB<U> &other) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] = other.m_data[i];
    }
  }

  bool operator == (const RGB &other) const {
    return memcmp(static_cast<const void *>(this), static_cast<const void *>(&other), sizeof(RGB)) == 0;
  }

  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  const U &As () const {
    static_assert(sizeof(U) == sizeof(RGB), "U must be a POD mapping directly onto this object");
    // TODO: somehow check that U is a POD consisting only of type ColorComponent
    return *reinterpret_cast<const U *>(this);
  }
  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename U>
  U &As () {
    static_assert(sizeof(U) == sizeof(RGB), "U must be a POD mapping directly onto this object");
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

  // Add another RGB value into this one, component-wise.  Note that this may
  // overflow the component range (which is defined for each valid component by
  // ColorComponent<T>::Zero() and ColorComponent<T>::One()), and for integral component
  // types, this will also involve value wrap-around.
  void operator += (const RGB &other) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] += other.m_data[i];
    }
  }
  RGB operator + (const RGB &other) const {
    RGB retval(*this);
    retval += other;
    return retval;
  }
  void operator *= (const RGB &other) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] *= other.m_data[i];
    }
  }
  RGB operator * (const RGB &other) {
    RGB retval(*this);
    retval *= other;
    return retval;
  }
  void operator *= (const ColorComponent<T> &masking_factor) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i] += masking_factor;
    }
  }
  RGB operator * (const ColorComponent<T> &masking_factor) {
    RGB retval(*this);
    retval *= masking_factor;
    return retval;
  }

  void Clamp () {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      ClampComponent(m_data[i]);
    }
  }
  RGB Clamped () const {
    RGB retval(*this);
    retval.Clamp();
    return retval;
  }

  void BlendWith (const RGB &blend_target, const ColorComponent<T> &blend_parameter) {
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      m_data[i].BlendWith(blend_target.m_data[i], blend_parameter);
    }
  }
  RGB BlendedWith (const RGB &blend_target, const ColorComponent<T> &blend_parameter) const {
    RGB retval(*this);
    retval.BlendWith(blend_target, blend_parameter);
    return retval;
  }

private:

  ColorComponent<T> m_data[COMPONENT_COUNT];
};
