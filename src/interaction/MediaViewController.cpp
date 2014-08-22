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

void MediaViewController::OnUserPlayPause() {
  if(m_avi) { m_avi->Mex }
}

void MediaViewController::OnUserChangedVolume(float value) {
  if(m_avi) {
    m_avi->SetVolume(value);
  }
}

void MediaViewController::OnVolumeChanged(float oldVolume, float newVolume) {
  
  if(m_mv) {
    m_mv->SetVolume(newVolume);
  }
}