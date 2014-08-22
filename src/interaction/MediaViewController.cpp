#include "stdafx.h"
#include "MediaViewController.h"
#include "osinterface/AudioVolumeInterface.h"
#include "uievents/AbstractVolumeControl.h"

MediaViewController::MediaViewController(void)
{
}

MediaViewController::~MediaViewController(void)
{
}

void MediaViewController::OnUserChangedVolume(float value) {
  float vol = m_avi->GetVolume();
  m_mv->SetVolume(vol);
}

void MediaViewController::OnVolumeChanged(float oldVolume, float newVolume) {
  if(m_avi)
    m_avi->SetVolume(newVolume);
}