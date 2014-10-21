#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace Hmd {

/// @brief This class is intended to be a greatest-common-divisor representation for vector/matrix values.
/// @details The ReinterpretAs and AlignmentAware_ReinterpretAs methods can be used to interpret
/// the orientation quaterinon as whatever type is desired (e.g. the Component_-valued 3-vector type
/// of your favorite linear algebra library).  Note that the requested type for these accessors
/// must be in standard layout and must map directly onto a SIZE_-element array of doubles.
template <typename Component_, std::size_t SIZE_>
class IntermediateArray : public std::array<Component_,SIZE_> {
public:

  static_assert(SIZE_ > 0, "Can't have a zero-sized IntermediateArray.");

  IntermediateArray () { }
  IntermediateArray (const std::array<Component_,SIZE_> &array) : std::array<Component_,SIZE_>(array) { }
  IntermediateArray (const IntermediateArray &array) : std::array<Component_,SIZE_>(array) { }
  template <typename T_>
  IntermediateArray (const T_ &t) {
    static_assert(std::is_standard_layout<T_>::value, "Requested type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(IntermediateArray), "Size of requested type does not match that of this class.");
    memcpy(static_cast<void *>(this), static_cast<const void *>(&t), sizeof(IntermediateArray));
  }

  /// @brief No-overhead const accessor for interpreting this IntermediateArray as whatever type is desired.
  /// @details The requested type must be in standard layout and must map directly onto
  /// this IntermediateArray type (i.e. a SIZE_-element array doubles).
  /// @note This method is not safe to use with types that have alignment requirements -- see
  /// AlignmentAware_ReinterpretAs.
  template <typename T_>
  const T_ &ReinterpretAs () const {
    static_assert(std::is_standard_layout<T_>::value, "Requested type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(IntermediateArray), "Size of requested type does not match that of this class.");
    return *reinterpret_cast<const T_ *>(this);
  }
  /// @brief No-overhead non-const accessor for interpreting this IntermediateArray as whatever type is desired.
  /// @details The requested type must be in standard layout and must map directly onto
  /// this IntermediateArray type (i.e. a SIZE_-element array doubles).
  /// @note This method is not safe to use with types that have alignment requirements -- see
  /// AlignmentAware_ReinterpretAs.
  template <typename T_>
  T_ &ReinterpretAs () {
    static_assert(std::is_standard_layout<T_>::value, "Requested type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(IntermediateArray), "Size of requested type does not match that of this class.");
    return *reinterpret_cast<T_ *>(this);
  }
  /// @brief Accessor for interpreting this IntermediateArray as whatever type is desired.
  /// @details The requested type must be in standard layout and must map directly onto
  /// this IntermediateArray type (i.e. a SIZE_-element array doubles).
  /// @note This method is safe to use with types that have alignment requirements, but incurs some
  /// overhead in the form of an operator=() assignment of this IntermediateArray object to another.
  template <typename T_>
  T_ AlignmentAware_ReinterpretAs () const {
    static_assert(std::is_standard_layout<T_>::value, "Requsted type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(IntermediateArray), "Size of requested type does not match that of this class.");
    // This union is designed to handle two things:
    // 1. Handling direct memory layout mapping between the A_ type and B_.
    // 2. Handling alignment requirements that A_ or B_ may have.
    union AlignedConverter {
      IntermediateArray input;
      T_ output;
    };
    AlignedConverter conv;
    conv.input = *this;
    return conv.output;
  }
};

} // end of namespace Hmd
