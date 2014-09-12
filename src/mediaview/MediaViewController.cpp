#include "stdafx.h"
#include "MediaViewController.h"
#include "InteractionConfigs.h"

#include "mediaview/MediaViewStateMachine.h"
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
    if (m_mediaViewStateMachine) {
      float systemVolume = m_audioVolumeInterface->GetVolume();
      m_volume = systemVolume;
      m_volume = std::min(1.0f, std::max(0.0f, m_volume));
      m_mediaViewStateMachine->SetViewVolume(systemVolume);
    }
  }
}

void MediaViewController::OnUserChangedVolume(float dVolume) {
  if ( m_audioVolumeInterface ) {
    m_volume += dVolume;
    m_volume = std::min(1.0f, std::max(0.0f, m_volume));
    m_audioVolumeInterface->SetVolume(m_volume);
  }
}

void MediaViewController::OnVolumeChanged(float oldVolume, float newVolume) {
  if (m_mediaViewStateMachine) {
    m_volume = newVolume;
    m_volume = std::min(1.0f, std::max(0.0f, m_volume));
    m_mediaViewStateMachine->SetViewVolume(m_volume);
  }
}
