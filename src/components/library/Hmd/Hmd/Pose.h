#pragma once

#include <array>
#include <cstring>
#include <type_traits>

namespace Leap {
namespace Hmd {

template <typename T> using Array2 = std::array<T,2>;
template <typename T> using Array3 = std::array<T,3>;
template <typename T> using Array4 = std::array<T,4>;

template <typename T>
class Pose {
public:

  Pose (const Array3<T> &position, const Array4<T> &orientation)
    :
    m_position(position),
    m_orientation(orientation)
  { }

  const Array3<T> &Position () const { return m_position; }
  Array3<T> &Position () { return m_position; }

  const Array4<T> &Orientation () const { return m_orientation; }
  Array4<T> &Orientation () { return m_orientation; }

  

private:

  // A positional 3-vector.
  Array3<T> m_position;
  // A quaternion-valued orientation.
  Array4<T> m_orientation;
};

} // end of namespace Hmd
} // end of namespace Leap
