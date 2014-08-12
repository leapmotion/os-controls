#include "stdafx.h"
#include "MediaControllerMac.h"

#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/NSEvent.h>
#include <Foundation/Foundation.h>
#include <IOKit/hidsystem/ev_keymap.h>

MediaController* MediaController::New(void) {
  return new MediaControllerMac;
}

void MediaControllerMac::PlayPause(void) {
  SendSpecialKeyEventPair(NX_KEYTYPE_PLAY);
}

void MediaControllerMac::Stop(void) {
  // No concept of Stop on Mac
}

void MediaControllerMac::Next(void) {
  SendSpecialKeyEventPair(NX_KEYTYPE_NEXT);
}

void MediaControllerMac::Prev(void) {
  SendSpecialKeyEventPair(NX_KEYTYPE_PREVIOUS);
}

void MediaControllerMac::VolumeUp(void) {
  // Include shift-key modifier so the control doesn't produce "pop" noise
  SendSpecialKeyEventPair(NX_KEYTYPE_SOUND_UP, NSShiftKeyMask);
}

void MediaControllerMac::VolumeDown(void) {
  // Include shift-key modifier so the control doesn't produce "pop" noise
  SendSpecialKeyEventPair(NX_KEYTYPE_SOUND_DOWN, NSShiftKeyMask);
}

void MediaControllerMac::Mute(void) {
  // Include shift-key modifier so the control doesn't produce "pop" noise
  SendSpecialKeyEventPair(NX_KEYTYPE_MUTE, NSShiftKeyMask);
}

void MediaControllerMac::SendSpecialKeyEvent(int32_t keyType, uint32_t mask, bool isDown) {
  const NSUInteger flags = (isDown ? NX_KEYDOWN : NX_KEYUP) << 8;
  const NSInteger data1 = (keyType << 16) | flags;

  // Create a system-defined event in order to send our special-key event
  NSEvent* event = [NSEvent otherEventWithType:NSSystemDefined
                            location:NSZeroPoint
                            modifierFlags:(flags | mask) // Include an optional "modifier flags" mask
                            timestamp:0
                            windowNumber:0
                            context:nullptr
                            subtype:NX_SUBTYPE_AUX_CONTROL_BUTTONS
                            data1:data1
                            data2:-1];
  if (!event) {
    return;
  }
  @autoreleasepool {
    CGEventRef eventRef = [event CGEvent];
    CGEventPost(kCGHIDEventTap, eventRef);
  }
  [event release];
}
