// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// Implements a two-state system with hysteresis.
/// An update via a point above the activation threshold yields an activated state.
/// An update below the persistence threshold yields an enervated state.
/// The state cannot change in the interval between persistence and activation thresholds.
/// </summary>
/// <remarks>
/// The Hysteresis can be visualized as a function equal to 1 above the activation threshold,
/// equal to 0 below the persistence threshold, and equal to a linear interpolation between.
/// Given an a configured Hysteresis
///  Hysteresis instance(persistance, activation);
/// this function can be evaluated for all points as follows:
///  instance(point).interpolated();
/// In order to for Hysteresis to apply a condition it must be the case that
///  ((persistence <= activation) && increasing) ||
///  ((persistence <= activation) && !increasing)
/// In the absence of conditions the state cannot change due to updates.
/// </remarks>
class Hysteresis {
  friend class HysteresisRange;
  template<int dim>
  friend class HysteresisVolume;

protected:
  bool m_activated;
  double m_interpolated;

public:
  bool increasing;
  double persistence;
  double activation;

  /// <summary>Default constructor applies no conditions and is enervated</summary>
  /// <remarks>
  /// Since the absence of conditions prevents updates from changing activation,
  /// the constructor allows the option of initializing in the active state.
  /// </remarks>
  Hysteresis(bool i_activated = false);

  /// <summary>Constructs a Hysteresis threshold, and is initialized in an enervated state</summary>
  Hysteresis(double i_persistence, double i_activation, bool i_activated = false);

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
  const Hysteresis& operator () (double point);

  /// <summary>Defines an activation threshold without hysteresis</summary>
  Hysteresis& operator = (const double& rhs);

  /// <summary>Combines Hysteresis conditions so that activation and enervation require lhs && rhs.</summary>
  /// <remarks>
  /// This combination always expands the persistence region.
  /// </remarks>
  void operator &= (const Hysteresis& rhs);

  /// <summary>Combines Hysteresis conditions so that activation and enervation require lhs || rhs.</summary>
  /// <remarks>
  /// This combination always retracts the persistence region, and can revoke the Hysteresis condition.
  /// </remarks>
  void operator |= (const Hysteresis& rhs);

  /// <summary>Displaces activation and persistence thresholds by +displace</summary>
  void operator += (double displace);
  /// <summary>Displaces activation and persistence thresholds by -displace</summary>
  void operator -= (double displace);

  /// <summary>
  /// Makes bool(*this) == true && double(*this) == 1.
  /// </summary>
  void activate();

  /// <summary>
  /// Makes bool(*this) == false && double(*this) == 0.
  /// </summary>
  void enervate();

  /// <returns>True if the point update will yield bool(*this) == true</returns>
  bool will_activate(double point) const;

  /// <returns>True if the point update will yield bool(*this) == false</returns>
  bool will_enervate(double point) const;

  /// <returns>Returns the number of conditions - either 0 or 1</returns>
  int conditions() const;
};
