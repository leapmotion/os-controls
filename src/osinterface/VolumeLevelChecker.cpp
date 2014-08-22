#include "stdafx.h"
#include "VolumeLevelChecker.h"
#include "AudioVolumeInterface.h"
#include "uievents/SystemMultimediaEventListener.h"

void VolumeLevelChecker::Update(std::chrono::duration<double> deltaT) {
  float volume = m_avi->GetVolume();
  if(m_oldVolume == volume)
    return;

  m_smel(&SystemMultimediaEventListener::OnVolumeChanged)(m_oldVolume, volume);
  m_oldVolume = volume;
}