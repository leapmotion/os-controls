#include "stdafx.h"
#include "MediaView.h"
#include "RenderState.h"
#include "limits.h"

#include <iostream>

const static float PI = 3.14159265f;

MediaView::MediaView(const Vector3& center, float offset) :
m_time(0),
m_opacity(0.0f)
{
  //TODO: Move this into a for loop that handles the sweep angle calculations
  m_wedges.resize(4);
  m_wedges[0] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, 5 * PI / 4, 7 * PI / 4, Vector3(0, -1 * offset, 0)); //top
  m_wedges[1] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, -PI / 4, PI / 4, Vector3(offset, 0, 0)); //right
  m_wedges[2] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, PI / 4, 3 * PI / 4, Vector3(0, offset, 0), true); //down
  m_wedges[3] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, 3 * PI / 4, 5 * PI / 4, Vector3(-1 * offset, 0, 0));  //left
  
  m_volumeControl = RenderEngineNode::Create<VolumeControl>(50 - 10 - offset, 10);

  Translation() = center;
}

void MediaView::InitChildren() {
  for(std::shared_ptr<RadialButton> radial : m_wedges) {
    AddChild(radial);
  }
  AddChild(m_volumeControl);
}

void MediaView::AnimationUpdate(const RenderFrame& frame) {
  m_time += static_cast<float>(frame.deltaT.count());
  
  for(auto wedge : m_wedges) {
    //TODO: Wedge Coloring and Such
    if ( m_activeWedge == wedge ) {
      wedge->Nudge(m_interactionDistance);
    }
    else
    {
      wedge->Nudge(0);
    }
  }

  //TODO: make this animate
  switch (m_fadeState) {
  case FADE_IN:
      if ( m_opacity < config::MEDIA_BASE_OPACITY ) {
        m_opacity = std::min(config::MEDIA_BASE_OPACITY, (float)(m_opacity + (2.0f * frame.deltaT.count())) );
    }
    setMenuOpacity(m_opacity);
    break;
  case FADE_OUT:
    if ( m_opacity > 0.0f ) {
      m_opacity = std::max(0.0f, (float)(m_opacity - (2.0 * frame.deltaT.count())) );
    }
    setMenuOpacity(m_opacity);
    break;
  case INVISIBLE:
    setMenuOpacity(0.0f);
    break;
  }
}

void MediaView::SetFadeState(FadeState newState){
  m_fadeState = newState;
}

float MediaView::Volume() {
  return m_volumeControl->Volume();
}

void MediaView::SetVolume(float volume) {
  m_volumeControl->SetVolume(volume);
}

void MediaView::NudgeVolume(float dVolume) {
  m_volumeControl->NudgeVolume(dVolume);
}

void MediaView::setMenuOpacity(float opacity) {
  m_wedges[0]->SetMaxOpacity(opacity);
  m_wedges[1]->SetMaxOpacity(opacity);
  m_wedges[3]->SetMaxOpacity(opacity);
  m_volumeControl->SetOpacity(opacity);
}

int MediaView::setActiveWedgeFromPoint(const Vector2& point) {
  int retVal = -1;
  float minDist = std::numeric_limits<float>::max();
  
  for(size_t i=0; i < m_wedges.size(); i++) {
    auto wedge = m_wedges[i];
    float dist = wedge->DistanceToCenter(point);
    if ( dist < minDist ) {
      minDist = dist;
      retVal = i;
      m_activeWedge = wedge;
    }
  }
  
  std::cout << std::endl;
  return static_cast<int>(retVal); // return the index of the selected wedge.
}

void MediaView::SetInteractionDistance(float distance) {
  m_interactionDistance = distance;
}

void MediaView::DeselectWedges() {
  m_activeWedge = nullptr;
}

void MediaView::CloseMenu(double selectionCloseDelayTime) {
  SetFadeState(FADE_OUT);
}

void MediaView::Move(const Vector3& coords) {
  Translation() = coords;
}
