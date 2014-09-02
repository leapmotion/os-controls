#pragma once
#include "WindowScroller.h"

enum {
  kIOHIDEventTypeNULL = 0,
  kIOHIDEventTypeVendorDefined = 1,
  kIOHIDEventTypeRotation = 5,
  kIOHIDEventTypeScroll = 6,
  kIOHIDEventTypeZoom = 8,
  kIOHIDEventTypeVelocity = 9,
  kIOHIDEventTypeDigitizer = 11,
  kIOHIDEventTypeMouse = 18,
  kIOHIDEventTypeDockSwipe = 23,
  kIOHIDEventTypeFluidTouchGesture = 27,
  kIOHIDEventTypeGestureBegan = 61,
  kIOHIDEventTypeGestureEnded = 62
};
typedef uint32_t IOHIDEventType;

enum {
  kIOHIDDigitizerEventRange      = 0x00000001,
  kIOHIDDigitizerEventTouch      = 0x00000002,
  kIOHIDDigitizerEventPosition   = 0x00000004,
  kIOHIDDigitizerEventStop       = 0x00000008,
  kIOHIDDigitizerEventPeak       = 0x00000010,
  kIOHIDDigitizerEventIdentity   = 0x00000020,
  kIOHIDDigitizerEventAttribute  = 0x00000040,
  kIOHIDDigitizerEventCancel     = 0x00000080,
  kIOHIDDigitizerEventStart      = 0x00000100,
  kIOHIDDigitizerEventResting    = 0x00000200,
  kIOHIDDigitizerEventSwipeUp    = 0x01000000,
  kIOHIDDigitizerEventSwipeDown  = 0x02000000,
  kIOHIDDigitizerEventSwipeLeft  = 0x04000000,
  kIOHIDDigitizerEventSwipeRight = 0x08000000,
  kIOHIDDigitizerEventSwipeMask  = 0xFF000000
};
typedef uint32_t IOHIDDigitizerEventMask;

enum {
  kIOHIDEventPhaseUndefined        = 0x00,
  kIOHIDEventPhaseBegan            = 0x01,
  kIOHIDEventPhaseChanged          = 0x02,
  kIOHIDEventPhaseEnded            = 0x04,
  kIOHIDEventPhaseCancelled        = 0x08,
  kIOHIDEventPhaseMayBegin         = 0x80,
  kIOHIDEventEventPhaseMask        = 0xFF,
  kIOHIDEventEventOptionPhaseShift = 24
};
typedef uint16_t IOHIDEventPhaseBits;

enum {
  kIOHIDEventMomentumPhaseUndefined = 0,
  kIOHIDEventMomentumPhaseBegan     = 1,
  kIOHIDEventMomentumPhaseChanged   = 2,
  kIOHIDEventMomentumPhaseEnded     = 3
};
typedef uint32_t IOHIDEventMomentumPhase;

class WindowScrollerMac:
  public IWindowScroller
{
public:
  WindowScrollerMac();
  ~WindowScrollerMac();

protected:
  void DoScrollBy(float deltaX, float deltaY, bool isMomentum) override;

private:
  CGEventRef CreateEvent(IOHIDEventType type) const;
  IOHIDDigitizerEventMask PhaseToEventMask(IOHIDEventPhaseBits phase) const;

  // Hold on to partial pixels that haven't yet been sent in an event
  OSPoint m_scrollPartialPixel;
  // Hold on to partial lines that haven't yet been sent in an event
  OSPoint m_scrollPartialLine;

  IOHIDEventPhaseBits m_phase;
  IOHIDEventMomentumPhase m_momentumPhase;
};
