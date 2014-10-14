#pragma once

#include "Leap/Hmd/IntermediateArray.h"

namespace Leap {
namespace Hmd {

template <std::size_t SIZE_>
using DoubleArray = IntermediateArray<double,SIZE_>;

enum class EyeName { LEFT, RIGHT };

class EyeConfiguration {
public:

  virtual ~EyeConfiguration () { }

  virtual Hmd::EyeName EyeName () const = 0;
  virtual DoubleArray<4*4> ProjectionMatrix () const = 0;
};

} // end of namespace Hmd
} // end of namespace Leap
