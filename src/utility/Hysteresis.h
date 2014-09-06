// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>A range defined by a coordinate interval</summary>
class Range {
public:
  double min;
  double max;

  Range():
    min(0.),
    max(0.)
  {}

  /// <returns>True if there exist points in Range</returns>
  operator bool () const {
    return (min < max);
  }

  /// <returns>True if the point is inside of the range</returns>
  bool operator () (double point) const {
    return min < point && point < max;
  }

  /// <returns>True if all points in the argument range are inside of this range</returns>
  /// <remarks>
  /// If the argument range is empty the result always true.
  /// </remarks>
  bool operator () (Range inside) const;

  const Range& operator += (double displace);
  const Range& operator -= (double displace);
  const Range& operator *= (double rescale);
  const Range& operator /= (double rescale);
};

/// <summary>
/// A two-state system with hysteresis.
/// </summary>
/// <remarks>
/// A small range defines the activation volume.
/// A larger enveloping range defines the persistence volume.
/// </remarks>
class Hysteresis {
protected:
  bool m_active;
  double m_interpolated;

public:
  Range activation;
  Range persistence;

  Hysteresis(Range i_activation = Range(), Range i_persistence = Range()):
    m_active(false),
    activation(i_activation),
    persistence(i_persistence)
  {}

  const Hysteresis& operator += (double displace);
  const Hysteresis& operator -= (double displace);
  const Hysteresis& operator *= (double rescale);
  const Hysteresis& operator /= (double rescale);

  /// <returns>True when the system is in the active state</returns>
  operator bool() const {
    return m_active;
  }

  /// <returns>Interpolation between active and inactive states</returns>
  /// <remarks>
  /// Interpolation is equal to 1 inside of the active region
  /// is equal to 0 outside of the persistence region
  /// and is the fraction of the distance between the regions otherwise.
  /// </remarks>
  operator double() const {
    return m_interpolated;
  }

  /// <summary>Updates the activation state according to the point argument</summary>
  /// <remarks>
  /// Return enables in-line comparison via cast to bool or double
  /// </remarks>
  const Hysteresis& operator () (double point);

  /// <summary>Makes state inactive</summary>
  void reset();
};
