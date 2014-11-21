#pragma once

#include "Leap/GL/Internal/ColorComponent.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Leap {
namespace GL {

template <typename T>
struct ColorComponent {

  static_assert(Internal::ComponentValueTraits<T>::IS_DEFINED, "Type T is not a valid underlying type for a color component.");
  static ColorComponent Zero () { return ColorComponent(Internal::ComponentValueTraits<T>::Min()); }
  static ColorComponent One () { return ColorComponent(Internal::ComponentValueTraits<T>::Max()); }

  // Construct a ColorComponent with uninitialized value.
  ColorComponent () { }
  // Construct a ColorComponent with type-specific value.  Note that the way that this value will
  // be interpreted depends on the dynamic range of the underlying type.  The values mapped
  // to 0 and 1 are given by the static methods Zero and One.
  ColorComponent (const T &value) : m_value(value) { }
  // Copy constructor.
  ColorComponent (const ColorComponent &other) : m_value(other.m_value) { }

  // Returns this component converted to use a different underlying type.  Note that
  // this may result in a loss of precision (e.g. ColorComponent<uint32_t>::As<uint16_t>).
  template <typename U>
  ColorComponent<U> AsComponent () const {
    ColorComponent<U> retval;
    Internal::ConvertComponentValue(m_value, retval.Value());
    return retval;
  }

  // It seems like the copy constructors take care of this.
  // const ColorComponent &operator = (const ColorComponent &other) { m_value = other.m_value; return *this; }
  // const ColorComponent &operator = (const T &value) { m_value = value; return *this; }

  const T &Value () const { return m_value; }
  T &Value () { return m_value; }
  operator const T & () const { return m_value; }
  operator T & () { return m_value; }

  // It seems like the conversion operator(s) take care of these.
  // bool operator == (const ColorComponent &other) const { return m_value == other.m_value; }
  // bool operator != (const ColorComponent &other) const { return m_value != other.m_value; }
  // bool operator <= (const ColorComponent &other) const { return m_value <= other.m_value; }
  // bool operator <  (const ColorComponent &other) const { return m_value <  other.m_value; }
  // bool operator >= (const ColorComponent &other) const { return m_value >= other.m_value; }
  // bool operator >  (const ColorComponent &other) const { return m_value >  other.m_value; }

  // Color component addition (combine lightnesses).
  void operator += (const ColorComponent &other) {
    m_value += other.m_value;
  }
  // Color component addition (combine lightnesses).
  ColorComponent operator + (const ColorComponent &other) {
    ColorComponent retval(*this);
    retval += other;
    return retval;
  }
  // Masks this ColorComponent with other.  This is really just multiplication of the ColorComponent
  // values if they are mapped onto the range [0,1].  Masking with 0 gives 0, and masking
  // a component value V with the value of One() gives V.
  void operator *= (const ColorComponent &other) {
    m_value = Internal::ComponentValueMask(m_value, other.m_value);
  }
  // This is the color component masking operation.
  ColorComponent operator * (const ColorComponent &other) const {
    ColorComponent retval(*this);
    retval *= other;
    return retval;
  }

  // Clamps this component to within its dynamic range.  For types T which use their
  // entire dynamic range (uint8_t, uint16_t, uint32_t, uint64_t), this is a no-op,
  // because it's impossible for the value to be outside the range.
  void Clamp () {
    typedef Internal::ComponentValueTraits<ColorComponent> ComponentValueTraits;
    m_value = std::min(std::max(m_value, Zero()), One());
  }
  // Returns the clamped value.
  ColorComponent Clamped () const {
    ColorComponent retval(*this);
    retval.Clamp();
    return retval;
  }

  // Linear blending between two Components, given a blending parameter (of the same
  // type).  Returns this ColorComponent if blend_parameter is Zero(), returns blend_target
  // if blend_parameter is One(), and linearly interpolates between the two in all other cases.
  void BlendWith (const ColorComponent &blend_target, const ColorComponent &blend_parameter) {
    ColorComponent one_minus_blend_parameter(One() - blend_parameter);
    m_value =   Internal::ComponentValueMask(m_value, one_minus_blend_parameter.m_value) 
              + Internal::ComponentValueMask(blend_target.m_value, blend_parameter.m_value);
  }
  // Linear blending between two Components.
  ColorComponent BlendedWith (const ColorComponent &blend_target, const ColorComponent &blend_parameter) {
    ColorComponent retval(*this);
    retval.BlendWith(blend_target, blend_parameter);
    return retval;
  }

private:

  T m_value;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
