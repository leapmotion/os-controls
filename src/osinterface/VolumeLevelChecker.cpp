#include "stdafx.h"
#include "VolumeLevelChecker.h"
#include "AudioVolumeInterface.h"
#include "uievents/SystemMultimediaEventListener.h"

VolumeLevelChecker::VolumeLevelChecker(void):
  m_oldMute(true),
  m_oldVolume(0.0f)
{

}

void VolumeLevelChecker::Tick(std::chrono::duration<double> deltaT) {
  if(!m_avi)
    // No audio-volume interface, we cannot raise any interrupts to higher tiers
    return;

  float volume = m_avi->GetVolume();
  if(m_oldVolume != volume) {
    m_smel(&SystemMultimediaEventListener::OnVolumeChanged)(m_oldVolume, volume);
    m_oldVolume = volume;
  }

  bool muted = m_avi->IsMuted();
  if(muted != m_oldMute) {
    m_smel(&SystemMultimediaEventListener::OnMuteChanged)(muted);
    m_oldMute = muted;
  }
}