#include "stdafx.h"
#include "MediaControllerMac.h"

MediaController* MediaController::New(void) {
  return new MediaControllerMac;
}

void MediaControllerMac::PlayPause(void) {
}

void MediaControllerMac::Stop(void) {
}

void MediaControllerMac::Next(void) {
}

void MediaControllerMac::Prev(void) {
}

void MediaControllerMac::VolumeUp(void) {
}

void MediaControllerMac::VolumeDown(void) {
}

void MediaControllerMac::Mute(void) {
}
