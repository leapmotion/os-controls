#include "stdafx.h"
#include "MediaViewController.h"
#include "graphics/MediaView.h"
#include "osinterface/AudioVolumeInterface.h"

MediaViewController::MediaViewController(void)
{
}

MediaViewController::~MediaViewController(void)
{
}

void MediaViewController::Update(double deltaT) {
  float vol = m_avi->GetVolume();
  m_mv->SetVolume(vol);
}

void MediaViewController::OnVolumeChanged(float volume) {
  if(m_avi)
    m_avi->SetVolume(volume);
}