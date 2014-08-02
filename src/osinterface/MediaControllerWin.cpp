#include "stdafx.h"
#include "MediaControllerWin.h"

MediaController* MediaController::New(void) {
  return new MediaControllerWin;
}

void MediaControllerWin::SendMediaKeyCode(uint32_t vk) {
  keybd_event(vk, 0x22, KEYEVENTF_EXTENDEDKEY, 0);
  keybd_event(vk, 0x22, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void MediaControllerWin::PlayPause(void) {
  SendMediaKeyCode(VK_MEDIA_PLAY_PAUSE);
}

void MediaControllerWin::Stop(void) {
  SendMediaKeyCode(VK_MEDIA_STOP);
}

void MediaControllerWin::Next(void) {
  SendMediaKeyCode(VK_MEDIA_NEXT_TRACK);
}

void MediaControllerWin::Prev(void) {
  SendMediaKeyCode(VK_MEDIA_PREV_TRACK);
}

void MediaControllerWin::VolumeUp(void) {
  SendMediaKeyCode(VK_VOLUME_UP);
}

void MediaControllerWin::VolumeDown(void) {
  SendMediaKeyCode(VK_VOLUME_DOWN);
}

void MediaControllerWin::Mute(void) {
  SendMediaKeyCode(VK_VOLUME_MUTE);
}

double MediaControllerWin::GetVolume(void) {
  return 0.0;
}