#include "stdafx.h"
#include "MediaViewController.h"
#include "graphics/MediaView.h"
#include "osinterface/MediaInterface.h"
#include "osinterface/AudioVolumeInterface.h"

MediaViewController::MediaViewController(void)
{
}

MediaViewController::~MediaViewController(void)
{
}

void MediaViewController::OnUserPlayPause() {
  if(m_audioVolumeInterface) { m_mediaInterface->PlayPause(); }
}

void MediaViewController::OnUserNextTrack() {
  if(m_audioVolumeInterface) { m_mediaInterface->Next(); }
}

void MediaViewController::OnUserPrevTrack() {
  if(m_audioVolumeInterface) { m_mediaInterface->Prev(); }
}


void MediaViewController::OnUserChangedVolume(float value) {
  if(m_audioVolumeInterface) {
    m_audioVolumeInterface->SetVolume(value);
  }
}

void MediaViewController::OnVolumeChanged(float oldVolume, float newVolume) {
  if(m_mediaView) {
    m_mediaView->SetVolumeView(newVolume);
  }
}