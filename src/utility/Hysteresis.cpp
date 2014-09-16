// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "Hysteresis.h"

Hysteresis::Hysteresis(bool i_activated):
  m_activated(i_activated),
  m_interpolated(i_activated ? 1. : 0.),
  increasing(true),
  persistence(0.),
  activation(-1.)
{}

Hysteresis::Hysteresis(double i_persistence, double i_activation, bool i_activated):
  m_activated(i_activated),
  m_interpolated(i_activated ? 1. : 0.),
  increasing(i_persistence <= i_activation),
  persistence(i_persistence),
  activation(i_activation)
{}

const Hysteresis& Hysteresis::operator () (double point) {
  if (will_activate(point)) {
    m_activated = true;
    m_interpolated = 1.;
  } else if (will_enervate(point)) {
    m_activated = false;
    m_interpolated = 0.;
  } else {
    m_interpolated = (point - persistence) / (activation - persistence);
  }
  return *this;
}

Hysteresis& Hysteresis::operator = (const double& rhs) {
  persistence = rhs;
  activation = rhs;
  return *this;
}

void Hysteresis::operator &= (const Hysteresis& rhs) {
  if (rhs.conditions() == 0)
    return;
  if (conditions() == 0) {
    *this = rhs;
    return;
  }

  if (rhs.increasing != increasing)
    return;

  // Logical && combination of activation and interpolation states
  m_activated &= rhs.m_activated;
  m_interpolated *= rhs.m_interpolated;

  // Activation and Enervation regions are the && of lhs and rhs conditions
  if (rhs.increasing) {
    if (activation < rhs.activation)
      activation = rhs.activation;
    if (persistence > rhs.persistence)
      persistence = rhs.persistence;
  } else {
    if (activation > rhs.activation)
      activation = rhs.activation;
    if (persistence < rhs.persistence)
      persistence = rhs.persistence;
  }
}

void Hysteresis::operator |= (const Hysteresis& rhs) {
  if (rhs.conditions() == 0)
    return;
  if (conditions() == 0) {
    *this = rhs;
    return;
  }

  if (rhs.increasing != increasing)
    return;

  // Logical || combination of activation and interpolation states
  m_activated |= rhs.m_activated;
  m_interpolated *= 1. - rhs.m_interpolated;
  m_interpolated += rhs.m_interpolated;

  // Activation and Enervation regions are the || of lhs and rhs conditions
  if (rhs.increasing) {
    if (activation > rhs.activation)
      activation = rhs.activation;
    if (persistence < rhs.persistence)
      persistence = rhs.persistence;
  } else {
    if (activation < rhs.activation)
      activation = rhs.activation;
    if (persistence > rhs.persistence)
      persistence = rhs.persistence;
  }
}

void Hysteresis::operator += (double displace) {
  activation += displace;
  persistence += displace;
}

void Hysteresis::operator -= (double displace) {
  activation -= displace;
  persistence -= displace;
}

void Hysteresis::activate() {
  m_activated = true;
  m_interpolated = 1.;
}

void Hysteresis::enervate() {
  m_activated = false;
  m_interpolated = 0.;
}

bool Hysteresis::will_activate(double point) const {
  return
  increasing?
  (persistence <= activation && activation < point):
  (activation <= persistence && point < activation);
}

bool Hysteresis::will_enervate(double point) const {
  return
  increasing?
  (persistence <= activation && point <= persistence):
  (activation <= persistence && persistence <= point);
}

int Hysteresis::conditions() const {
  return
  increasing ?
  (persistence <= activation ? 1 : 0) :
  (activation <= persistence ? 1 : 0);
}
