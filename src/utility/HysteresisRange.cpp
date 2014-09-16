// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "HysteresisRange.h"

HysteresisRange::HysteresisRange(bool i_activated):
  m_activated(i_activated),
  m_interpolated(i_activated ? 1. : 0.),
  m_min(0.,-1.,false),
  m_max(0.,1.,false)
{
  m_min.increasing = true;
  m_max.increasing = false;
}


HysteresisRange::HysteresisRange(const Hysteresis& i_mid, bool i_activated):
  m_activated(i_activated),
  m_interpolated(i_activated ? 1. : 0.),
  m_min(i_mid),
  m_max(i_mid)
{
  m_min.increasing = true;
  m_max.increasing = false;
  update();
}

HysteresisRange::HysteresisRange(const Hysteresis& i_min, const Hysteresis& i_max, bool i_activated):
  m_activated(i_activated),
  m_interpolated(i_activated ? 1. : 0.),
  m_min(i_min),
  m_max(i_max)
{
  m_min.increasing = true;
  m_max.increasing = false;
  update();
}

void HysteresisRange::update() {
  int total = conditions();
  if (total > 1) {
    m_activated = bool(m_min) && bool(m_max);
    m_interpolated = double(m_min) * double(m_max);
  } else if (total > 0) {
    if (m_min.conditions() > 0) {
      m_activated = m_min;
      m_interpolated = m_min;
    }
    if (m_max.conditions() > 0) {
      m_activated = m_max;
      m_interpolated = m_max;
    }
  }
}

const HysteresisRange& HysteresisRange::operator () (double point) {
  m_min(point);
  m_max(point);
  update();
  return *this;
}

HysteresisRange& HysteresisRange::operator = (const Hysteresis& rhs) {
  // Makes the thresholds identical - only one will apply a condition
  m_min = rhs;
  m_min.increasing = true;
  m_max = rhs;
  m_max.increasing = false;
  update();
  return *this;
}

void HysteresisRange::operator &= (const Hysteresis& rhs) {
  if (rhs.increasing)
    m_min &= rhs;
  if (!rhs.increasing)
    m_max &= rhs;
  update();
}

void HysteresisRange::operator &= (const HysteresisRange& rhs) {
  m_min &= rhs.m_min;
  m_max &= rhs.m_max;
  update();
}

void HysteresisRange::operator |= (const Hysteresis& rhs) {
  if (rhs.increasing)
    m_min |= rhs;
  if (!rhs.increasing)
    m_max |= rhs;
  update();
}

void HysteresisRange::operator |= (const HysteresisRange& rhs) {
  m_min |= rhs.m_min;
  m_max |= rhs.m_max;
  update();
}

void HysteresisRange::operator += (double displace) {
  m_min += displace;
  m_max += displace;
}

void HysteresisRange::operator -= (double displace) {
  m_min -= displace;
  m_max -= displace;
}

void HysteresisRange::set_min(double i_persistence, double i_activation) {
  m_min.persistence = i_persistence;
  m_min.activation = i_activation;
}

void HysteresisRange::set_max(double i_persistence, double i_activation) {
  m_max.persistence = i_persistence;
  m_max.activation = i_activation;
}

void HysteresisRange::activate() {
  m_activated = m_min.m_activated = m_max.m_activated = true;
  m_interpolated = m_min.m_interpolated = m_max.m_interpolated = 1.;
}

void HysteresisRange::enervate() {
  m_activated = m_min.m_activated = m_max.m_activated = false;
  m_interpolated = m_min.m_interpolated = m_max.m_interpolated = 0.;
}

bool HysteresisRange::will_activate(double point) const {
  bool change = conditions() > 0;
  if (m_min.conditions() > 0)
    change &= m_min.will_activate(point);
  if (m_min.conditions() > 0)
    change &= m_min.will_activate(point);
  return change;
}

bool HysteresisRange::will_enervate(double point) const {
  bool change = conditions() > 0;
  if (m_min.conditions() > 0)
    change |= m_min.will_enervate(point);
  if (m_min.conditions() > 0)
    change |= m_min.will_enervate(point);
  return change;
}

int HysteresisRange::conditions() const {
  return m_min.conditions() + m_max.conditions();
}
