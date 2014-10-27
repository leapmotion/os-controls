#pragma once

#include "IntermediateArray.h"
#include "MatrixComponentOrder.h"
#include <type_traits>

namespace Hmd {

template <std::size_t SIZE_>
using DoubleArray = IntermediateArray<double,SIZE_>;

enum class QuaternionNormalization { NOT_REQUIRED, REQUIRED };

class IEyeConfiguration;

/// @brief Represents a position and orientation in 3-space.
class IPose {
public:

  virtual ~IPose () { }

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
  /// @details The return value is a 4x4 matrix in block form, where R is a 3x3 orthogonal matrix
  /// representing the orientation, and T is a column vector representing the position.
  /// @verbatim
  /// [ R_00 R_01 R_02 T_0 ]
  /// [ R_10 R_11 R_12 T_1 ]
  /// [ R_20 R_21 R_22 T_2 ]
  /// [ 0    0    0    1   ]
  /// @endverbatim
  /// The ReinterpretAs<T_> and AlignmentAware_ReinterpretAs methods of DoubleArray can be used
  /// to interpret the position as whatever type is desired (e.g. the double-valued 4x4 matrix
  /// type of your favorite linear algebra library).
  virtual DoubleArray<4*4> TotalMatrix (MatrixComponentOrder matrix_component_order) const = 0;
  
  // TODO document
  virtual DoubleArray<4*4> ViewMatrix (MatrixComponentOrder matrix_component_order, const IEyeConfiguration &eye_configuration) const = 0;

  // These are really just sanity checks.
  static_assert(std::is_standard_layout<DoubleArray<3>>::value, "DoubleArray should be in standard layout.");
  static_assert(std::is_standard_layout<DoubleArray<4>>::value, "DoubleArray should be in standard layout.");
  static_assert(std::is_standard_layout<DoubleArray<3*3>>::value, "DoubleArray should be in standard layout.");
  static_assert(std::is_standard_layout<DoubleArray<4*4>>::value, "DoubleArray should be in standard layout.");
};

} // end of namespace Hmd
