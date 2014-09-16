// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Hysteresis.h"

template<int dim>
class HysteresisVolume;

/// <summary>
/// Implements Hysteresis with optional min and max boundaries.
/// Activation is the logical && of both activation regions.
/// Enervation is the logical || of both enervation regions.
/// </sumary>
/// <remarks>
/// In the absence of conditions the state cannot change due to updates.
/// It is possible to put the system into a configuration where it can remain active,
/// but can only transition to an enervated state.
/// </remarks>
class HysteresisRange {
  template<int dim>
  friend class HysteresisVolume;
  
protected:
  /// CONTRACT:
  /// m_activated is the && of all conditions
  /// m_interpolated is the * of all conditions
  bool m_activated;
  double m_interpolated;
  void update();

  /// CONTRACT:
  /// m_min.increasing == true
  /// m_max.increasing == false
  /// When both min and max conditions apply: min.activation < max.activation
  Hysteresis m_min;
  Hysteresis m_max;

public:
  /// <remarks>Default constructor applies no conditions and is enervated</remarks>
  HysteresisRange(bool i_activated = false);

  /// <remarks>Construction with one condition</remarks>
  HysteresisRange(const Hysteresis& i_mid, bool i_activated = false);

  /// <remarks>Construction with two conditions</remarks>
  HysteresisRange(const Hysteresis& i_min, const Hysteresis& i_max, bool i_activated = false);

  /// <summary>Implicit cast to activation state</summary>
  /// <returns>True when the system is in the active state</returns>
  operator bool() const {
    return m_activated;
  }

  /// <summary>Implicit cast to interpolating function</summary>
  /// <returns>Interpolation between active and inactive states</returns>
  operator double() const {
    return m_interpolated;
  }

  /// <summary>Updates the activation state according to the point argument</summary>
  /// <remarks>
  /// Return enables in-line comparison to updated state via cast to bool or double.
  /// </remarks>
  const HysteresisRange& operator () (double point);

  /// <summary>Makes the range equivalent to the single-direction Hysteresis rhs</summary>
  HysteresisRange& operator = (const Hysteresis& rhs);

  /// <summary>Combines conditions of lhs and rhs using logical && on independent conditions</sumary>
  /// <remarks>This has the ability to revoke both conditions</remarks>
  void operator &= (const Hysteresis& rhs);
  /// <summary>Combines conditions of lhs and rhs using logical && on independent conditions</sumary>
  /// <remarks>This has the ability to revoke both conditions</remarks>
  void operator &= (const HysteresisRange& rhs);

  /// <summary>Combines conditions of lhs and rhs using logical || on independent conditions</sumary>
  void operator |= (const Hysteresis& rhs);
  /// <summary>Combines conditions of lhs and rhs using logical || on independent conditions</sumary>
  void operator |= (const HysteresisRange& rhs);

  /// <summary>Displaces activation and persistence thresholds by +displace</summary>
  void operator += (double displace);
  /// <summary>Displaces activation and persistence thresholds by -displace</summary>
  void operator -= (double displace);

  /// <summary>Sets the persistence and activation thresholds for the minimum condition</summary>
  /// <remarks>This has the ability to revoke the minimum condition or both conditions</remarks>
  void set_min(double i_persistence, double i_activation);
  /// <summary>Sets the persistence and activation thresholds for the maximum condition</summary>
  /// <remarks>This has the ability to revoke the maximum condition or both conditions</remarks>
  void set_max(double i_persistence, double i_activation);

  /// <returns>The minimum thresholds, including individual activation</returns>
  Hysteresis get_min() const {
    return m_min;
  }


  /// <returns>The maximum thresholds, including individual activation</returns>
  Hysteresis get_max() const {
    return m_max;
  }

  /// <summary>Makes bool(*this) == true && double(*this) == 1.</summary>
  void activate();

  /// <summary>Makes bool(*this) == false && double(*this) == 0.</summary>
  void enervate();

  /// <returns>True if point will ensure bool(*this) == true</returns>
  bool will_activate(double point) const;

  /// <returns>True if point will ensure bool(*this) == false</returns>
  bool will_enervate(double point) const;

  /// <returns>The number of conditions applied to the state, from 0 to 2</returns>
  /// <remarks>
  /// When there are no points that could yield an active state there are no conditions.
  /// </remarks>
  int conditions() const;
};
