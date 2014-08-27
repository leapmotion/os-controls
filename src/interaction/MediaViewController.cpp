#include "stdafx.h"
#include "MediaViewController.h"
#include "graphics/MediaView.h"
#include "osinterface/MediaInterface.h"
#include "osinterface/AudioVolumeInterface.h"

MediaViewController::MediaViewController(void)
{
  if(m_audioVolumeInterface) {
    m_volume = m_audioVolumeInterface->GetVolume();
  } else {
    m_volume = 0.0f;
  }
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

void MediaViewController::OnInitializeVolume() {
  if(m_audioVolumeInterface) {
    AutowiredFast<MediaView> mediaView;
    if (mediaView) {
      float systemVolume = m_audioVolumeInterface->GetVolume();
      m_volume = systemVolume;
      mediaView->SetVolumeView(systemVolume);
    }
  }
}

void MediaViewController::OnUserChangedVolume(float dVolume) {
  if(m_audioVolumeInterface) {
    m_volume += dVolume;
    m_audioVolumeInterface->SetVolume(m_volume);
  }
}

void MediaViewController::OnVolumeChanged(float oldVolume, float newVolume) {
  AutowiredFast<MediaView> mediaView;
  if (mediaView) {
    m_volume = newVolume;
    mediaView->SetVolumeView(newVolume);
  }
}
