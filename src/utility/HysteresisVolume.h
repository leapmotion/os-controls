// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "HysteresisRange.h"

/// <summary>
/// Implements HysteresisRange conditions on multiple coordinates.
/// </summary>
/// <remarks>
/// There are two ways in which HysteresisStacks can be combined:
/// * when the stacks describe conditions on disinct coordinates.
/// & when the stacks describe conditions on the same coordinates.
/// In both cases, the system becomes active only when all components
/// are active, and becomes ennervated only when all components are enervated.
/// </remarks>
template<int dim>
class HysteresisVolume {
  template<int add>
  friend class HysteresisVolume;

protected:
  /// CONTRACT:
  /// m_activated is the && of all conditions
  /// m_interpolated is the * of all conditions
  bool m_activated;
  double m_interpolated;
  void update() {
    if (conditions() > 0) {
      m_activated = true;
      m_interpolated = 1.;
      for (HysteresisRange& axis : m_space) {
        m_activated &= bool(axis);
        m_interpolated *= double(axis);
      }
    }
  }

  HysteresisRange m_space[dim];

public:
  /// <remarks>Default construct yields no conditions and enervated state</remarks>
  HysteresisVolume(bool i_activated = false):
    m_activated(i_activated),
    m_interpolated(i_activated ? 1. : 0.)
  {}

  /// <remarks>Initial state is enervated with interpolation = 0.</remarks>
  HysteresisVolume(const HysteresisRange i_space[dim], bool i_activated = false):
    m_activated(i_activated),
    m_interpolated(i_activated ? 1. : 0.)
  {
    for (int ind = 0; ind < dim; ++ind)
      m_space[ind] = i_space[ind];
    update();
  }

  /// <summary>
  /// Implicit cast to activation state
  /// </summary>
  /// <returns>True when the system is in the active state</returns>
  operator bool() const {
    return m_activated;
  }

  /// <summary>
  /// Implicit cast to interpolating function
  /// </summary>
  /// <returns>Interpolation between active and inactive states</returns>
  operator double() const {
    return m_interpolated;
  }

  /// <summary>
  /// Updates the activation state according to the point argument.
  /// </summary>
  /// <remarks>
  /// Return enables in-line comparison to updated state via cast to bool or double.
  /// </remarks>
  const HysteresisVolume& operator () (double point[dim]) {
    for (int ind = 0; ind < dim; ++ind)
      m_space[ind](point[ind]);
    update();
    return *this;
  }

  /// <summary>Displaces activation and persistence thresholds by +displace</summary>
  void operator += (double displace[dim]) {
    for (HysteresisRange& axis : m_space)
      axis += displace;
  }
  /// <summary>Displaces activation and persistence thresholds by -displace</summary>
  void operator -= (double displace[dim]) {
    for (HysteresisRange& axis : m_space)
      axis -= displace;
  }

  /// <summary>Combines conditions of lhs and rhs using logical && on independent conditions</sumary>
  void operator &= (HysteresisRange rhs[dim]) {
    for (int ind = 0; ind < dim; ++ind)
      m_space[ind] &= rhs[ind];
    update();
  }
  /// <summary>Combines conditions of lhs and rhs using logical && on independent conditions</sumary>
  void operator &= (const HysteresisVolume<dim>& rhs) {
    for (int ind = 0; ind < dim; ++ind)
      m_space[ind] &= rhs.m_space[ind];
    update();
  }

  /// <summary>Combines conditions of lhs and rhs using logical || on independent conditions</sumary>
  void operator |= (HysteresisRange rhs[dim]) {
    for (int ind = 0; ind < dim; ++ind)
      m_space[ind] |= rhs[ind];
    update();
  }
  /// <summary>Combines conditions of lhs and rhs using logical || on independent conditions</sumary>
  void operator |= (const HysteresisVolume<dim>& rhs) {
    for (int ind = 0; ind < dim; ++ind)
      m_space[ind] |= rhs.m_space[ind];
    update();
  }

  /// <summary>Cartesian product of lhs and rhs spaces, with lhs providing lower indices</sumary>
  template<int add>
  HysteresisVolume<dim + add> operator * (const HysteresisVolume<add>& rhs) {
    HysteresisVolume<dim + add> join;
    for (int ind = 0; ind < dim; ++ind) {
      join.m_space[ind] = m_space[ind];
    }
    for (int ind = 0; ind < add; ++ind) {
      join.m_space[dim + ind] = rhs.m_space[ind];
    }
    join.m_activated = m_activated && rhs.m_activated;
    join.m_interpolated = m_activated && rhs.m_interpolated;
    return join;
  }

  /// <summary>HysteresisRange indexing</summary>
  const HysteresisRange& operator [] (int index) const {
    return m_space[index];
  }
  /// <summary>HysteresisRange indexing</summary>
  HysteresisRange& operator [] (int index) {
    return m_space[index];
  }

  /// <summary>Makes bool(*this) == true && double(*this) == 1.</summary>
  void activate() {
    m_activated = true;
    m_interpolated = 1.;
    for (HysteresisRange& axis : m_space)
      axis.activate();
  }

  /// <summary>Makes bool(*this) == false && double(*this) == 0.</summary>
  void enervate() {
    m_activated = false;
    m_interpolated = 0.;
    for (HysteresisRange& axis : m_space)
      axis.enervate();
  }

  /// <returns>True if point will ensure bool(*this) == true</returns>
  bool will_activate(double point) const {
    bool change = conditions() > 0;
    for (HysteresisRange& axis : m_space)
      if (axis.conditions() > 0)
        change &= axis.will_activate(point);
    return change;
  }

  /// <returns>True if point will ensure bool(*this) == false</returns>
  bool will_enervate(double point) const {
    bool change = conditions() > 0;
    for (HysteresisRange& axis : m_space)
      if (axis.conditions() > 0)
        change |= axis.will_activate(point);
    return change;
  }

  /// <returns>The number of conditions applied to the state</returns>
  /// <remarks> 0 <= conditions() <= 2 </remarks>
  int conditions() const {
    int total = 0;
    for (const HysteresisRange& axis : m_space)
      total += axis.conditions();
    return total;
  }

  /// <summary>Number of dimensions to which HysteresisRange conditions are applied</summary>
  static const int dimensions = dim;
};
