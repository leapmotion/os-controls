#include "stdafx.h"
#include "AROverlay.h"
#include "osinterface/RenderWindow.h"
#include "hmdinterface/IDevice.h"
#include "hmdinterface/IDeviceConfiguration.h"

static const double animationDuration = .2; //in seconds

AROverlay::AROverlay() :
m_shouldDisplayOverlay(false),
m_overlayOffset(),
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

  if (m_shouldDisplayOverlay) {
    if (m_wipeDirection == SystemWipe::Direction::DOWN) {
      m_mainView->SetClip(0, 0, screenWidth, m_overlayOffset.Current());
    }
    else{
      m_mainView->SetClip(0, maxHeight - m_overlayOffset.Current(), screenWidth, m_overlayOffset.Current());
    }
  }
  else {
    if (m_wipeDirection == SystemWipe::Direction::DOWN) {
      m_mainView->SetClip(0, maxHeight - m_overlayOffset.Current(), screenWidth, m_overlayOffset.Current());
    }
    else {
      m_mainView->SetClip(0, 0, screenWidth, m_overlayOffset.Current());
    }
  }

  m_compEngine->CommitChanges();
}

//AutoFilter methods (to be informed of system wipe occuring
void AROverlay::AutoFilter(const SystemWipe& wipe) {
  if (wipe.status == SystemWipe::Status::BEGIN || wipe.status == SystemWipe::Status::ABORT) {
    m_shouldDisplayOverlay = !m_shouldDisplayOverlay;
    m_wipeDirection = wipe.direction;

    if (m_shouldDisplayOverlay) {
      m_overlayOffset.Set(m_overlayWindow->GetSize().height);
    }
    else {
      m_overlayOffset.Set(0);
    }
  }
  else if (wipe.status == SystemWipe::Status::UPDATE) {
    m_overlayOffset.SetCompletion(wipe.progress);
  }

  m_lastWipe = wipe;
}