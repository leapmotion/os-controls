// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "Hysteresis.h"

bool Range::operator () (Range inside) const {
  if (!inside)
    return true;
  return min <= inside.min && inside.max <= max;
}

const Range& Range::operator += (double displace) {
  min += displace;
  max += displace;
  return *this;
}

const Range& Range::operator -= (double displace) {
  min -= displace;
  max -= displace;
  return *this;
}

const Range& Range::operator *= (double rescale) {
  min *= rescale;
  max *= rescale;
  return *this;
}

const Range& Range::operator /= (double rescale) {
  min /= rescale;
  max /= rescale;
  return *this;
}

const Hysteresis& Hysteresis::operator += (double displace) {
  activation += displace;
  persistence += displace;
  return *this;
}
const Hysteresis& Hysteresis::operator -= (double displace) {
  activation -= displace;
  persistence -= displace;
  return *this;
}
const Hysteresis& Hysteresis::operator *= (double rescale) {
  activation *= rescale;
  persistence *= rescale;
  return *this;
}
const Hysteresis& Hysteresis::operator /= (double rescale) {
  activation /= rescale;
  persistence /= rescale;
  return *this;
}

const Hysteresis& Hysteresis::operator () (double point) {
  if (activation(point)) {
    m_active = true;
    m_interpolated = 1.;
  } else if (!persistence(point)) {
    m_active = false;
    m_interpolated = 0.;
  } else {
    m_interpolated = 0.; //Interpolation when surfaces coincide
    if (persistence.min < point && point <= activation.min) {
      m_interpolated = (point - persistence.min) / (activation.min - persistence.min);
    }
    if (activation.max <= point && point < persistence.max) {
      m_interpolated = (point - persistence.max) / (activation.max - persistence.max);
    }
  }
  return *this;
}

void Hysteresis::reset() {
  m_active = false;
  m_interpolated = 0.;
}
