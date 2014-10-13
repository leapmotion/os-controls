#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace Leap {
namespace Hmd {

/// @brief This class is intended to be a greatest-common-divisor representation for vector/matrix values.
/// @details The ReinterpretAs and AlignmentAware_ReinterpretAs methods can be used to interpret
/// the orientation quaterinon as whatever type is desired (e.g. the double-valued 3-vector type
/// of your favorite linear algebra library).  Note that the requested type for these accessors
/// must be in standard layout and must map directly onto a SIZE_-element array of doubles.
template <std::size_t SIZE_>
class DoubleArray : public std::array<double,SIZE_> {
public:

  DoubleArray () { }
  DoubleArray (const std::array<double,SIZE_> &array) : std::array<double,SIZE_>(array) { }
  DoubleArray (const DoubleArray &array) : std::array<double,SIZE_>(array) { }
  template <typename T_>
  DoubleArray (const T_ &t) {
    static_assert(std::is_standard_layout<T_>::value, "Requested type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(DoubleArray), "Size of requested type does not match that of this class.");
    memcpy(static_cast<void *>(this), static_cast<const void *>(&t), sizeof(DoubleArray));
  }

  /// @brief No-overhead const accessor for interpreting this DoubleArray as whatever type is desired.
  /// @details The requested type must be in standard layout and must map directly onto
  /// this DoubleArray type (i.e. a SIZE_-element array doubles).
  /// @note This method is not safe to use with types that have alignment requirements -- see
  /// AlignmentAware_ReinterpretAs.
  template <typename T_>
  const T_ &ReinterpretAs () const {
    static_assert(std::is_standard_layout<T_>::value, "Requested type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(DoubleArray), "Size of requested type does not match that of this class.");
    return *reinterpret_cast<const T_ *>(this);
  }
  /// @brief No-overhead non-const accessor for interpreting this DoubleArray as whatever type is desired.
  /// @details The requested type must be in standard layout and must map directly onto
  /// this DoubleArray type (i.e. a SIZE_-element array doubles).
  /// @note This method is not safe to use with types that have alignment requirements -- see
  /// AlignmentAware_ReinterpretAs.
  template <typename T_>
  T_ &ReinterpretAs () {
    static_assert(std::is_standard_layout<T_>::value, "Requested type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(DoubleArray), "Size of requested type does not match that of this class.");
    return *reinterpret_cast<T_ *>(this);
  }
  /// @brief Accessor for interpreting this DoubleArray as whatever type is desired.
  /// @details The requested type must be in standard layout and must map directly onto
  /// this DoubleArray type (i.e. a SIZE_-element array doubles).
  /// @note This method is safe to use with types that have alignment requirements, but incurs some
  /// overhead in the form of an operator=() assignment of this DoubleArray object to another.
  template <typename T_>
  T_ AlignmentAware_ReinterpretAs () const {
    static_assert(std::is_standard_layout<T_>::value, "Requsted type must be in standard layout.");
    static_assert(sizeof(T_) == sizeof(DoubleArray), "Size of requested type does not match that of this class.");
    // This union is designed to handle two things:
    // 1. Handling direct memory layout mapping between the A_ type and B_.
    // 2. Handling alignment requirements that A_ or B_ may have.
    union AlignedConverter {
      DoubleArray input;
      T_ output;
    };
    AlignedConverter conv;
    conv.input = *this;
    return conv.output;
  }
};

enum class QuaternionNormalization { NOT_REQUIRED = 0, REQUIRED };
enum class MatrixComponentOrder { ROW_MAJOR = 0, COLUMN_MAJOR };

/// @brief Represents a position and orientation in 3-space.
class Pose {
public:

  virtual ~Pose () { }

  /// @brief Returns the position.
  /// @details The ReinterpretAs<T_> and AlignmentAware_ReinterpretAs methods of DoubleArray
  /// can be used to interpret the position as whatever type is desired (e.g. the double-valued
  /// 3-vector type of your favorite linear algebra library).
  virtual DoubleArray<3> Position () const = 0;
  /// @brief Returns the orientation as a quaternion with the given normalization requirement.
  /// @details The ReinterpretAs<T_> and AlignmentAware_ReinterpretAs methods of DoubleArray
  /// can be used to interpret the orientation quaterinon as whatever type is desired (e.g. the
  /// double-valued quaternion type of your favorite linear algebra library).
  /// @note A quaternion w+ix+jy+kz should be stored in the order (w,x,y,z).  This is different
  /// from OVR::Quat<T>.
  virtual DoubleArray<4> OrientationQuaternion (QuaternionNormalization quaternion_normalization) const = 0;
  /// @brief Returns the orientation as a 3x3 matrix in the given row/column-major ordering.
  /// @details The ReinterpretAs<T_> and AlignmentAware_ReinterpretAs methods of DoubleArray
  /// can be used to interpret the orientation matrix as whatever type is desired (e.g. the
  /// double-valued 3x3 matrix type of your favorite linear algebra library).
  virtual DoubleArray<3*3> OrientationMatrix (MatrixComponentOrder matrix_component_order) const = 0;
  /// @brief Returns the position and orientation as a 4x4 matrix which acts on the homogeneous space (x,y,z,1).
  /// @details The return value is a 4x4 matrix in block form:
  /// @verbatim
  /// [ R_00 R_01 R_02 T_0 ]
  /// [ R_10 R_11 R_12 T_1 ]
  /// [ R_20 R_21 R_22 T_2 ]
  /// [ 0    0    0    1   ]
  /// #endverbatim
  /// The ReinterpretAs<T_> and AlignmentAware_ReinterpretAs methods of DoubleArray can be used
  /// to interpret the position as whatever type is desired (e.g. the double-valued 4x4 matrix
  /// type of your favorite linear algebra library).
  virtual DoubleArray<4*4> TotalMatrix (MatrixComponentOrder matrix_component_order) const = 0;

  static_assert(std::is_standard_layout<DoubleArray<3>>::value, "DoubleArray should be in standard layout.");
  static_assert(std::is_standard_layout<DoubleArray<4>>::value, "DoubleArray should be in standard layout.");
  static_assert(std::is_standard_layout<DoubleArray<3*3>>::value, "DoubleArray should be in standard layout.");
  static_assert(std::is_standard_layout<DoubleArray<4*4>>::value, "DoubleArray should be in standard layout.");
};

} // end of namespace Hmd
} // end of namespace Leap
