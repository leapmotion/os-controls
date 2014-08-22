#include "stdafx.h"
#include "MediaView.h"
#include "RenderFrame.h"
#include "RenderState.h"
#include "limits.h"
#include "uievents/OSCDomain.h"
#include <iostream>

const static float PI = 3.14159265f;

MediaView::MediaView(const Vector3& center, float offset) :
m_opacity(0.0f, 0.5, EasingFunctions::QuadInOut<float>)
{
  //TODO: Move this into a for loop that handles the sweep angle calculations
  m_wedges.resize(4);
  m_wedges[0] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, 5 * PI / 4, 7 * PI / 4, Vector3(0, -1 * offset, 0)); //top
  m_wedges[1] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, -PI / 4, PI / 4, Vector3(offset, 0, 0)); //right
  m_wedges[2] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, PI / 4, 3 * PI / 4, Vector3(0, offset, 0), true); //down
  m_wedges[3] = RenderEngineNode::Create<RadialButton>(50 - offset, 100 - offset, 3 * PI / 4, 5 * PI / 4, Vector3(-1 * offset, 0, 0));  //left
  
  m_volumeControl = RenderEngineNode::Create<VolumeControl>(50 - 10 - offset, 10.0f);

  Translation() = center;
}

void MediaView::InitChildren() {
  for(std::shared_ptr<RadialButton> radial : m_wedges) {
    AddChild(radial);
  }
  AddChild(m_volumeControl);
}

void MediaView::AnimationUpdate(const RenderFrame& frame) {
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

  m_opacity.Update(frame.deltaT.count());
  setMenuOpacity(m_opacity.Current());
}

void MediaView::SetGoalOpacity(float goalOpacity) {
  m_opacity.Set(goalOpacity);
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

void MediaView::AutoFilter(OSCState state, const HandLocation& handLocation, const HandPose& handPose) {
  if(state != OSCState::MediaMenuVisible)
    return;

  // At this point, you need to figure out what to do with your input.  You might need to decide
  // which sliders need to be engaged, you might need to decide whether you are going to transition
  // states, or even possibly that an event has to be fired here.
  float volumeNudgeAmount = 0.012f;



  /// TODO:  Decide what state we're in


  // Only try to nudge the volume if we're not presently interacting with any wedges
  if(m_state == State::Zero) {
    if(volumeNudgeAmount > 0.0001f) {
      NudgeVolume(volumeNudgeAmount);
      m_mve(&MediaViewEventListener::OnUserChangedVolume)(volumeNudgeAmount);
    }
  }
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
  FadeOut();
}

void MediaView::Move(const Vector3& coords) {
  Translation() = coords;
}
