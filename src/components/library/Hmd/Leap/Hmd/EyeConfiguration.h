#pragma once

#include "Leap/Hmd/IntermediateArray.h"
#include "Leap/Hmd/MatrixComponentOrder.h"

namespace Leap {
namespace Hmd {

template <std::size_t SIZE_>
using DoubleArray = IntermediateArray<double,SIZE_>;

enum class EyeName { LEFT, RIGHT };

class EyeConfiguration {
public:

  virtual ~EyeConfiguration () { }

  virtual Hmd::EyeName EyeName () const = 0;
  /// @brief Returns the projection matrix for this eye configuration.
  /// @details The matrix is returned as a flat array, where the matrix_component_order
  /// parameter specifies that the array elements should be layed out in row-major or
  /// column-major order.
  virtual DoubleArray<4*4> ProjectionMatrix (double near_clip, double far_clip, MatrixComponentOrder matrix_component_order) const = 0;
  // TODO: potentially could add FOV data here
};

} // end of namespace Hmd
} // end of namespace Leap
