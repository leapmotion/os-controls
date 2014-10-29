#include "stdafx.h"
#include "AROverlay.h"
#include "osinterface/RenderWindow.h"
#include "hmdinterface/IDevice.h"
#include "hmdinterface/IDeviceConfiguration.h"

static const double defaultAnimationDuration = .2; //in seconds
static const double maxAnimationDuration = .6; //in seconds
static const double gestureCompletionCoverage = 1.0 / 3.0; //% of screen covered when gesture is registered as complete - subjective

AROverlay::AROverlay() :
m_wipeDisabled(false),
m_overlayOffset(0.f),
m_overlayWindow(RenderWindow::New())
{
  m_compDisplay.reset(m_compEngine->CreateDisplay(m_overlayWindow->GetSystemHandle()));
  m_mainView.reset(m_compEngine->CreateView());
  
  m_compDisplay->SetView(m_mainView.get());

  m_overlayWindow->SetTransparent(true);
  m_overlayWindow->SetVisible(true);
}

AROverlay::~AROverlay()
{
}

void AROverlay::SetSourceWindow(RenderWindow& window, bool copyDimensions) {
  m_mainView->SetContent(window.GetSystemHandle());
  window.SetCloaked();
  if (copyDimensions)
    m_overlayWindow->SetRect(window.GetRect());

  m_compEngine->CommitChanges();
}

void AROverlay::SetOverlayWindowRect(float x, float y, float width, float height) {
  m_overlayWindow->SetRect({ x, y, width, height });
}

//Updateable overrides
void AROverlay::Tick(std::chrono::duration<double> deltaT) {
  m_overlayWindow->ProcessEvents();

  const auto screenWidth = m_overlayWindow->GetSize().width;
  const auto maxHeight = m_overlayWindow->GetSize().height;

  //If we're not getting updates, finish the animation ourselves
  if (m_lastWipe.status == SystemWipe::Status::NOT_ACTIVE && m_overlayOffset.Completion() != 1.0)
    m_overlayOffset.Update(deltaT.count());

  //If isDisplayingOverlay && m_wipeDirection == Down | !isDisplayingOverlay && m_wipeDirection == Up
  // then the animation is playing from top to bottom
  //If isDisplayingOverlay && m_WipeDirection == Up | !isDisplayingOverlay && m_wipedirection == Down
  // then the animation is playing from the bottom up.
  if (isDisplayingOverlay() == (m_wipeDirection == SystemWipe::Direction::DOWN)) {
    m_mainView->SetClip(0, 0, screenWidth, m_overlayOffset.Current());
  }
  else {
    m_mainView->SetClip(0, maxHeight - m_overlayOffset.Current(), screenWidth, m_overlayOffset.Current());
  }
  m_compEngine->CommitChanges();
}

//AutoFilter methods (to be informed of system wipe occuring
void AROverlay::AutoFilter(const SystemWipe& wipe, const Leap::Frame& frame) {
  m_lastWipe = wipe;

  //Disable this behavior if we have recognized hands.
  if (frame.hands().count() > 0) {
    m_wipeDisabled = true;
  }
  else if (m_lastWipe.status == SystemWipe::Status::NOT_ACTIVE) {
    //Only re-enable if the recognizer isn't in the middle of something..
    m_wipeDisabled = false;
  }

  if (m_wipeDisabled) {
    if (m_lastWipe.status == SystemWipe::Status::UPDATE)
      m_lastWipe.status = SystemWipe::Status::ABORT;
    else
      m_lastWipe.status = SystemWipe::Status::NOT_ACTIVE;
  }

  if (m_lastWipe.status == SystemWipe::Status::BEGIN || m_lastWipe.status == SystemWipe::Status::ABORT) {
    const float newOverlayOffsetGoal = isDisplayingOverlay() ? 0 : m_overlayWindow->GetSize().height;
    m_overlayOffset.Set(newOverlayOffsetGoal, defaultAnimationDuration);
   
    //Grab keyboard focus if the overlay is getting displayed!
    m_overlayWindow->SetKBFocus(isDisplayingOverlay());
  }

  if (m_lastWipe.status == SystemWipe::Status::BEGIN) {
    m_wipeDirection = m_lastWipe.direction;
    m_wipeStart = std::chrono::steady_clock::now();
  }
  else if (m_lastWipe.status == SystemWipe::Status::UPDATE) {
    m_overlayOffset.SetCompletion(m_lastWipe.progress * gestureCompletionCoverage);
  }
  else if (m_lastWipe.status == SystemWipe::Status::COMPLETE || m_lastWipe.status == SystemWipe::Status::ABORT) {
    double wipeDuration = std::chrono::duration<double>(std::chrono::steady_clock::now() - m_wipeStart).count();
    //If the gesture took Y seconds to cover X% of the screen, then covering the rest should take
    // (1 - X) * (Y / X) seconds
    const double remainingTime = (1 - gestureCompletionCoverage) * (wipeDuration / gestureCompletionCoverage);
    m_overlayOffset.Set(m_overlayOffset.Goal(), std::min(remainingTime,maxAnimationDuration));
  }

}