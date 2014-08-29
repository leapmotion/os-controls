#include "stdafx.h"
#include "WindowScrollerMac.h"

#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/NSEvent.h>
#include <Foundation/NSProcessInfo.h>

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerMac;
}

WindowScrollerMac::WindowScrollerMac() :
  m_phase(kIOHIDEventPhaseUndefined),
  m_momentumPhase(kIOHIDEventMomentumPhaseUndefined)
{
  m_pixelsPerLine.x = 10;
  m_pixelsPerLine.y = 10;
}

WindowScrollerMac::~WindowScrollerMac()
{
  DoScrollBy(0.0f, 0.0f, true); // Abruptly cancel any existing scroll
}

void WindowScrollerMac::DoScrollBy(float deltaX, float deltaY, bool isMomentum) {
  CGEventRef event;

  if (isMomentum) {
    // If we were scrolling and just now switched to momentum scrolling, end our gesture
    if (m_phase == kIOHIDEventPhaseBegan || m_phase == kIOHIDEventPhaseChanged) {
      // On last (non-momentum) scroll, send a "Gesture Ended" event
      m_phase = kIOHIDEventPhaseEnded;
      event = CreateEvent(kIOHIDEventTypeGestureEnded);
      CGEventSetIntegerValueField(event, 115, kIOHIDEventTypeScroll); // 115: ended gesture subtype
      CGEventSetIntegerValueField(event, 123, kIOHIDGestureMotionNone);
      CGEventPost(kCGHIDEventTap, event);
      CFRelease(event);
      m_phase = kIOHIDEventPhaseUndefined;
      // If we don't have any momentum when we finish scrolling, do not attempt any momentum
      if (std::abs(deltaX) < FLT_EPSILON && std::abs(deltaY) < FLT_EPSILON) {
        return;
      }
      m_momentumPhase = kIOHIDEventMomentumPhaseBegan;
    } else if (m_momentumPhase == kIOHIDEventMomentumPhaseChanged) {
      // If we are applying momentum, and the momentum ends, let's end our momentum
      if (std::abs(deltaX) < FLT_EPSILON && std::abs(deltaY) < FLT_EPSILON) {
        m_momentumPhase = kIOHIDEventMomentumPhaseEnded;
      }
    } else if (m_momentumPhase == kIOHIDEventMomentumPhaseUndefined) {
      return;
    }
  } else if (m_phase == kIOHIDEventPhaseUndefined) {
    // On first scroll, send a "Gesture Began" event
    event = CreateEvent(kIOHIDEventTypeGestureBegan);
    CGEventSetIntegerValueField(event, 115, kIOHIDEventTypeScroll); // 115: begin gesture subtype
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    // We are now ready to send Scroll Gesture events
    m_phase = kIOHIDEventPhaseBegan;
  }

  const float ppi = 120.0f; // Pixels per inch (base this on the DPI of the monitors -- FIXME)
  const float ppmm = ppi/25.4f; // Convert pixels per inch to pixels per millimeter
  float px = deltaX*ppmm, py = deltaY*ppmm; // Convert to pixels
  float lx = px/m_pixelsPerLine.x, ly = py/m_pixelsPerLine.y; // Convert to lines

  // Adjust partial pixels
  m_scrollPartialPixel.x += px;
  m_scrollPartialPixel.y += py;
  px = round(m_scrollPartialPixel.x);
  py = round(m_scrollPartialPixel.y);
  m_scrollPartialPixel.x -= px;
  m_scrollPartialPixel.y -= py;

  // Adjust partial lines
  m_scrollPartialLine.x += lx;
  m_scrollPartialLine.y += ly;
  lx = floor(m_scrollPartialLine.x);
  ly = floor(m_scrollPartialLine.y);
  m_scrollPartialLine.x -= lx;
  m_scrollPartialLine.y -= ly;

  const int ilx = static_cast<int>(lx);
  const int ily = static_cast<int>(ly);

  // Scroll Gesture Event (only when gesturing)
  event = CreateEvent(kIOHIDEventTypeScroll);
  CGEventSetDoubleValueField(event, 113, px);
  CGEventSetDoubleValueField(event, 119, py);
  CGEventSetIntegerValueField(event, 123, 0x80000000); // Swipe direction
  CGEventSetIntegerValueField(event, 132, m_phase);
  CGEventSetIntegerValueField(event, 135, 1); // Unsure what this does
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);

  // Scroll Wheel Event
  event = CGEventCreateScrollWheelEvent(0, kCGScrollEventUnitPixel, 2, 0, 0);
  if (ily != 0) {
    CGEventSetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1, ily);
    CGEventSetIntegerValueField(event, kCGScrollWheelEventFixedPtDeltaAxis1, ily*65536);
  }
  if (ilx != 0) {
    CGEventSetIntegerValueField(event, kCGScrollWheelEventDeltaAxis2, ilx);
    CGEventSetIntegerValueField(event, kCGScrollWheelEventFixedPtDeltaAxis2, ilx*65536);
  }
  CGEventSetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis1, py);
  CGEventSetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis2, px);

  CGEventSetIntegerValueField(event, 99, m_phase); // phase
  CGEventSetIntegerValueField(event, 123, m_momentumPhase); // momentum phase
  CGEventSetIntegerValueField(event, 137, 1); // Unsure what this does
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);

  // Change phase or momentum phase as needed
  if (m_phase == kIOHIDEventPhaseBegan) {
    m_phase = kIOHIDEventPhaseChanged;
  } else if (m_momentumPhase == kIOHIDEventMomentumPhaseBegan) {
    m_momentumPhase = kIOHIDEventMomentumPhaseChanged;
  } else if (m_momentumPhase == kIOHIDEventMomentumPhaseEnded) {
    m_momentumPhase = kIOHIDEventMomentumPhaseUndefined;
  }
}

CGEventRef WindowScrollerMac::CreateEvent(IOHIDEventType type) const
{
  CGEventRef event = CGEventCreate(0);
  CGEventSetLocation(event, m_virtualPosition);
  CGEventTimestamp time = [[NSProcessInfo processInfo] systemUptime]*1000000000;
  CGEventSetTimestamp(event, time);
  CGEventSetFlags(event, 0x100);
  CGEventSetType(event, NSEventTypeGesture);
  CGEventSetIntegerValueField(event, 110, type); // 110: event subtype
  if (type != kIOHIDEventTypeGestureBegan &&
      type != kIOHIDEventTypeGestureEnded) {
    CGEventSetIntegerValueField(event, 133, PhaseToEventMask(m_phase)); // 133: digitizer event mask
  }
  return event;
}

IOHIDDigitizerEventMask WindowScrollerMac::PhaseToEventMask(IOHIDEventPhaseBits phase) const
{
  IOHIDDigitizerEventMask mask = 0;
  if (phase & kIOHIDEventPhaseBegan) {
    mask |= kIOHIDDigitizerEventStart;
  }
  if (phase & kIOHIDEventPhaseEnded) {
    mask |= kIOHIDDigitizerEventStop;
  } else {
    if (mask) {
      mask |= kIOHIDDigitizerEventPosition;
    } else {
      mask = 0;
    }
  }
  return mask;
}
