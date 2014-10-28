#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <type_traits>

// TODO: move this Internal namespace into a separate file.

// The contents of the Internal namespace are not intended to be used publicly, and provide
// no guarantee as to the stability of their API.  The classes and functions are used
// internally in the implementation of the publicly-presented classes.
namespace Internal {

// This metafunction defines the dynamic range (min, max) for types of the form ColorComponent<U>,
// as well as if the component type is integral or not.
template <typename T> struct ComponentValueTraits { static const bool IS_DEFINED = false; };

template <> struct ComponentValueTraits<uint8_t> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = true;
  static uint8_t Min () { return std::numeric_limits<uint8_t>::min(); } // Should be 0.
  static uint8_t Max () { return std::numeric_limits<uint8_t>::max(); }
};
template <> struct ComponentValueTraits<uint16_t> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = true;
  static uint16_t Min () { return std::numeric_limits<uint16_t>::min(); } // Should be 0.
  static uint16_t Max () { return std::numeric_limits<uint16_t>::max(); }
};
template <> struct ComponentValueTraits<uint32_t> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = true;
  static uint32_t Min () { return std::numeric_limits<uint32_t>::min(); } // Should be 0.
  static uint32_t Max () { return std::numeric_limits<uint32_t>::max(); }
};
template <> struct ComponentValueTraits<uint64_t> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = true;
  static uint64_t Min () { return std::numeric_limits<uint64_t>::min(); } // Should be 0.
  static uint64_t Max () { return std::numeric_limits<uint64_t>::max(); }
};
template <> struct ComponentValueTraits<float> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = false;
  static float Min () { return static_cast<float>(0); }
  static float Max () { return static_cast<float>(1); }
};
template <> struct ComponentValueTraits<double> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = false;
  static double Min () { return static_cast<double>(0); }
  static double Max () { return static_cast<double>(1); }
};
template <> struct ComponentValueTraits<long double> {
  static const bool IS_DEFINED = true;
  static const bool IS_INTEGRAL = false;
  static long double Min () { return static_cast<long double>(0); }
  static long double Max () { return static_cast<long double>(1); }
};

// Conversion functions -- convert between the ranges of the different component types

template <typename From, typename To>
typename std::enable_if<std::is_same<From,To>::value,
                        void>::type
  ConvertComponentValue (From from, To &to)
{
  static_assert(ComponentValueTraits<From>::IS_DEFINED, "From is not a valid color component type");
  static_assert(ComponentValueTraits<To>::IS_DEFINED,   "To is not a valid color component type");
  to = from;
}

template <typename From, typename To>
typename std::enable_if<!std::is_same<From,To>::value &&
                        std::is_floating_point<From>::value &&
                        std::is_floating_point<To>::value,
                        void>::type
  ConvertComponentValue (From from, To &to)
{
  static_assert(ComponentValueTraits<From>::IS_DEFINED, "From is not a valid color component type");
  static_assert(ComponentValueTraits<To>::IS_DEFINED,   "To is not a valid color component type");
  to = static_cast<To>(from);
}

// Convert small uints to big uints.

inline void ConvertComponentValue (uint8_t from, uint16_t &to) {
  // Use the base-2^8 digit 'from' as each base-2^8 digit of 'to'.
  to = (uint16_t(from) << 8) | uint16_t(from);
}

inline void ConvertComponentValue (uint8_t from, uint32_t &to) {
  // Use the base-2^8 digit 'from' as each base-2^8 digit of 'to'.
  to = (uint32_t(from) << 8) | uint32_t(from); // Populate the lower half.
  to |= to << 16;                              // Copy it into the upper half.
}

inline void ConvertComponentValue (uint8_t from, uint64_t &to) {
  // Use the base-2^8 digit 'from' as each base-2^8 digit of 'to'.
  to = (uint64_t(from) << 8) | uint64_t(from); // Populate the lower quarter.
  to |= to << 16;                              // Copy it into the second to lowest quarter.
  to |= to << 32;                              // Copy that into the upper half.
}

inline void ConvertComponentValue (uint16_t from, uint32_t &to) {
  // Use the base-2^16 digit 'from' as each base-2^16 digit of 'to'.
  to = (uint32_t(from) << 16) | uint32_t(from);
}

inline void ConvertComponentValue (uint16_t from, uint64_t &to) {
  // Use the base-2^16 digit 'from' as each base-2^16 digit of 'to'.
  to = (uint64_t(from) << 16) | uint64_t(from); // Populate the lower half.
  to |= to << 32;                               // Copy it into the upper half.
}

inline void ConvertComponentValue (uint32_t from, uint64_t &to) {
  // Use the base-2^32 digit 'from' as each base-2^32 digit of 'to'.
  to = (uint64_t(from) << 32) | uint64_t(from);
}

// Convert big uints to small uints.  TODO: figure out if there should be symmetric 
// rounding or not.  Right now it always rounds down.

inline void ConvertComponentValue (uint64_t from, uint8_t &to) {
  to = uint8_t(from >> 56);
}

inline void ConvertComponentValue (uint32_t from, uint8_t &to) {
  to = uint8_t(from >> 24);
}

inline void ConvertComponentValue (uint16_t from, uint8_t &to) {
  to = uint8_t(from >> 8);
}

inline void ConvertComponentValue (uint64_t from, uint16_t &to) {
  to = uint16_t(from >> 48);
}

inline void ConvertComponentValue (uint32_t from, uint16_t &to) {
  to = uint16_t(from >> 16);
}

inline void ConvertComponentValue (uint64_t from, uint32_t &to) {
  to = uint32_t(from >> 32);
}

// Convert from uints to floats.

inline void ConvertComponentValue (uint8_t from, float &to) {
  to = float(from) / float(ComponentValueTraits<uint8_t>::Max());
}

inline void ConvertComponentValue (uint16_t from, float &to) {
  to = float(from) / float(ComponentValueTraits<uint16_t>::Max());
}

inline void ConvertComponentValue (uint32_t from, float &to) {
  to = float(double(from) / double(ComponentValueTraits<uint32_t>::Max()));
}

inline void ConvertComponentValue (uint64_t from, float &to) {
  typedef long double Ld;
  to = float(Ld(from) / Ld(ComponentValueTraits<uint64_t>::Max()));
}

inline void ConvertComponentValue (uint8_t from, double &to) {
  to = double(from) / double(ComponentValueTraits<uint8_t>::Max());
}

inline void ConvertComponentValue (uint16_t from, double &to) {
  to = double(from) / double(ComponentValueTraits<uint16_t>::Max());
}

inline void ConvertComponentValue (uint32_t from, double &to) {
  to = double(from) / double(ComponentValueTraits<uint32_t>::Max());
}

inline void ConvertComponentValue (uint64_t from, double &to) {
  typedef long double Ld;
  to = double(Ld(from) / Ld(ComponentValueTraits<uint64_t>::Max()));
}

inline void ConvertComponentValue (uint8_t from, long double &to) {
  typedef long double Ld;
  to = Ld(from) / Ld(ComponentValueTraits<uint8_t>::Max());
}

inline void ConvertComponentValue (uint16_t from, long double &to) {
  typedef long double Ld;
  to = Ld(from) / Ld(ComponentValueTraits<uint16_t>::Max());
}

inline void ConvertComponentValue (uint32_t from, long double &to) {
  typedef long double Ld;
  to = Ld(from) / Ld(ComponentValueTraits<uint32_t>::Max());
}

inline void ConvertComponentValue (uint64_t from, long double &to) {
  typedef long double Ld;
  to = Ld(from) / Ld(ComponentValueTraits<uint64_t>::Max());
}

// Convert from floats to uints.

inline void ConvertComponentValue (float from, uint8_t &to) {
  to = uint8_t(from * float(ComponentValueTraits<uint8_t>::Max()));
}

inline void ConvertComponentValue (float from, uint16_t &to) {
  to = uint16_t(from * float(ComponentValueTraits<uint16_t>::Max()));
}

inline void ConvertComponentValue (float from, uint32_t &to) {
  to = uint32_t(double(from) * double(ComponentValueTraits<uint32_t>::Max()));
}

inline void ConvertComponentValue (float from, uint64_t &to) {
  typedef long double Ld;
  to = uint64_t(Ld(from) * Ld(ComponentValueTraits<uint64_t>::Max()));
}

inline void ConvertComponentValue (double from, uint8_t &to) {
  to = uint8_t(from * double(ComponentValueTraits<uint8_t>::Max()));
}

inline void ConvertComponentValue (double from, uint16_t &to) {
  to = uint16_t(from * double(ComponentValueTraits<uint16_t>::Max()));
}

inline void ConvertComponentValue (double from, uint32_t &to) {
  to = uint32_t(from * double(ComponentValueTraits<uint32_t>::Max()));
}

inline void ConvertComponentValue (double from, uint64_t &to) {
  typedef long double Ld;
  to = uint64_t(Ld(from) * Ld(ComponentValueTraits<uint64_t>::Max()));
}

inline void ConvertComponentValue (long double from, uint8_t &to) {
  typedef long double Ld;
  to = uint8_t(from * Ld(ComponentValueTraits<uint8_t>::Max()));
}

inline void ConvertComponentValue (long double from, uint16_t &to) {
  typedef long double Ld;
  to = uint16_t(from * Ld(ComponentValueTraits<uint16_t>::Max()));
}

inline void ConvertComponentValue (long double from, uint32_t &to) {
  typedef long double Ld;
  to = uint32_t(from * Ld(ComponentValueTraits<uint32_t>::Max()));
}

inline void ConvertComponentValue (long double from, uint64_t &to) {
  typedef long double Ld;
  to = uint64_t(from * Ld(ComponentValueTraits<uint64_t>::Max()));
}

// Masking operations.  The mask of max with n should be n, the mask of 0 with n
// should be 0, and it should be bilinear in its parameters.

template <typename ColorComponent>
typename std::enable_if<std::is_floating_point<ColorComponent>::value,ColorComponent>::type
  ComponentValueMask (ColorComponent a, ColorComponent b)
{
  static_assert(ComponentValueTraits<ColorComponent>::IS_DEFINED, "ColorComponent is not a valid color component type");
  return a * b;
}

inline uint8_t ComponentValueMask (uint8_t a, uint8_t b) {
  return uint8_t(uint16_t(a) * uint16_t(b) / uint16_t(ComponentValueTraits<uint8_t>::Max()));
}

inline uint16_t ComponentValueMask (uint16_t a, uint16_t b) {
  return uint16_t(uint32_t(a) * uint32_t(b) / uint32_t(ComponentValueTraits<uint16_t>::Max()));
}

inline uint32_t ComponentValueMask (uint32_t a, uint32_t b) {
  return uint32_t(uint64_t(a) * uint64_t(b) / uint64_t(ComponentValueTraits<uint32_t>::Max()));
}

inline uint64_t ComponentValueMask (uint64_t a, uint64_t b) {
  // Unfortunately there is no uint128_t (yet), so do this in long double precision
  // in order to have any chance that the computation can be done losslessly.
  typedef long double Ld;
  return uint64_t(Ld(a) * Ld(b) / Ld(ComponentValueTraits<uint64_t>::Max()));
}

} // end of namespace Internal

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
    m_value = m_value*one_minus_blend_parameter + blend_target*blend_parameter;
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
