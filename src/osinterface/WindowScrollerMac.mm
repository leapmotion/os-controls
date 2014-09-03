#include "stdafx.h"
#include "WindowScrollerMac.h"

#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/NSEvent.h>
#include <Foundation/NSProcessInfo.h>

#include <thread>
#include <cassert>

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerMac;
}

WindowScrollerMac::WindowScrollerMac() :
  m_scrollPartialPixel(OSPointZero),
  m_scrollPartialLine(OSPointZero),
  m_phase(kIOHIDEventPhaseUndefined),
  m_momentumPhase(kIOHIDEventMomentumPhaseUndefined)
{
}

WindowScrollerMac::~WindowScrollerMac() {
  DoScrollBy(0.0f, 0.0f, true); // Abruptly cancel any existing scroll
}

void WindowScrollerMac::DoScrollBy(float deltaX, float deltaY, bool isMomentum) {
  OSPoint deltaPixel = OSPointMake(deltaX*m_ppmm, deltaY*m_ppmm); // Convert to pixels
  OSPoint deltaLine  = OSPointMake(deltaPixel.x/10.0f, deltaPixel.y/10.f); // Convert to lines

  // Adjust partial pixels
  m_scrollPartialPixel.x += deltaPixel.x;
  m_scrollPartialPixel.y += deltaPixel.y;
  deltaPixel.x = round(m_scrollPartialPixel.x);
  deltaPixel.y = round(m_scrollPartialPixel.y);
  m_scrollPartialPixel.x -= deltaPixel.x;
  m_scrollPartialPixel.y -= deltaPixel.y;

  // Adjust partial lines
  m_scrollPartialLine.x += deltaLine.x;
  m_scrollPartialLine.y += deltaLine.y;
  deltaLine.x = floor(m_scrollPartialLine.x);
  deltaLine.y = floor(m_scrollPartialLine.y);
  m_scrollPartialLine.x -= deltaLine.x;
  m_scrollPartialLine.y -= deltaLine.y;

  CGEventRef event;

  if (isMomentum) {
    // If we were scrolling and just now switched to momentum scrolling, end our gesture
    if (m_phase == kIOHIDEventPhaseBegan || m_phase == kIOHIDEventPhaseChanged) {
      // Since we don't know when the last non-momentum scroll is coming,
      // interpret the first momentum scroll as the last non-momentum scroll.
      m_phase = kIOHIDEventPhaseEnded;
    } else if (m_momentumPhase == kIOHIDEventMomentumPhaseChanged ||
               m_momentumPhase == kIOHIDEventMomentumPhaseBegan) {
      // If we are applying momentum, and the momentum ends, let's end our momentum
      if (std::abs(deltaPixel.y) < FLT_EPSILON && std::abs(deltaPixel.x) < FLT_EPSILON) {
        m_momentumPhase = kIOHIDEventMomentumPhaseEnded;
      }
    } else if (m_momentumPhase == kIOHIDEventMomentumPhaseUndefined) {
      return;
    }
  } else if (m_phase == kIOHIDEventPhaseUndefined) {
    // We should never be in the middle of performing momentum when being asked to begin a new scroll
    assert(m_momentumPhase == kIOHIDEventMomentumPhaseUndefined);
    // On first scroll, send a "Gesture Began" event
    event = CreateEvent(kIOHIDEventTypeGestureBegan);
    CGEventSetIntegerValueField(event, 115, kIOHIDEventTypeScroll); // 115: begin gesture subtype
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    // We are now ready to send Scroll Gesture events
    m_phase = kIOHIDEventPhaseBegan;
  }
  const int ilx = static_cast<int>(deltaLine.x);
  const int ily = static_cast<int>(deltaLine.y);

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
  CGEventSetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis1, deltaPixel.y);
  CGEventSetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis2, deltaPixel.x);
  CGEventSetIntegerValueField(event, 99, m_phase); // phase
  CGEventSetIntegerValueField(event, 123, m_momentumPhase); // momentum phase
  CGEventSetIntegerValueField(event, 137, 1); // Unsure what this does
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);

  if (m_phase != kIOHIDEventPhaseUndefined) {
    // Scroll Gesture Event (only when gesturing)
    event = CreateEvent(kIOHIDEventTypeScroll);
    CGEventSetDoubleValueField(event, 113, deltaPixel.x);
    CGEventSetDoubleValueField(event, 119, deltaPixel.y);
    CGEventSetIntegerValueField(event, 123, 0x80000000); // Swipe direction
    CGEventSetIntegerValueField(event, 132, m_phase);
    CGEventSetIntegerValueField(event, 135, 1); // Unsure what this does
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
  }

  // Change phase or momentum phase as needed
  if (m_phase == kIOHIDEventPhaseBegan) {
    m_phase = kIOHIDEventPhaseChanged;
  } else if (m_phase == kIOHIDEventPhaseEnded) {
    m_phase = kIOHIDEventPhaseUndefined;
    // Delay between sending last scroll event and end gesture event, otherwise it isn't always detected -- FIXME
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    event = CreateEvent(kIOHIDEventTypeGestureEnded);
    CGEventSetIntegerValueField(event, 115, kIOHIDEventTypeScroll); // 115: ended gesture subtype
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    // If we don't have any momentum, there is nothing to do. Otherwise, start momentum phase
    if (std::abs(deltaPixel.y) < FLT_EPSILON && std::abs(deltaPixel.x) < FLT_EPSILON) {
      m_momentumPhase = kIOHIDEventMomentumPhaseUndefined;
    } else {
      m_momentumPhase = kIOHIDEventMomentumPhaseBegan;
    }
  } else if (m_momentumPhase == kIOHIDEventMomentumPhaseBegan) {
    m_momentumPhase = kIOHIDEventMomentumPhaseChanged;
  } else if (m_momentumPhase == kIOHIDEventMomentumPhaseEnded) {
    m_momentumPhase = kIOHIDEventMomentumPhaseUndefined;
  }
}

CGEventRef WindowScrollerMac::CreateEvent(IOHIDEventType type) const
{
  CGEventRef event = CGEventCreate(0);
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
