#include "stdafx.h"
#include "ExposeViewController.h"
#include "ExposeView.h"
#include "osinterface/OSWindow.h"

ExposeViewController::ExposeViewController() {}

void ExposeViewController::OnCreate(OSWindow& window) {
  std::shared_ptr<OSWindow> windowPtr = window.shared_from_this();
  if(m_windows.count(windowPtr))
    // Already have this entry, no reason to hit this again
    return;
  m_windows[windowPtr] = m_exposeView->NewExposeWindow(window);
}

void ExposeViewController::OnDestroy(OSWindow& window) {
  auto q = m_windows.find(window.shared_from_this());
  if(q == m_windows.end())
    // Short-circuit, we can't find this window in our map
    return;

  // Tell ExposeView that the window is gone, and that shutdown operations on this window should
  // take place.
  m_exposeView->RemoveExposeWindow(q->second);
}

void ExposeViewController::OnResize(OSWindow& window) {
  auto q = m_windows.find(window.shared_from_this());
  if(q == m_windows.end())
    // Short-circuit, we can't find this window in our map
    return;

  // Tell ExposeView that this window needs to be updated
  m_exposeView->UpdateExposeWindow(q->second);
}

void ExposeViewController::onWindowSelected(ExposeViewWindow& wnd) {
  // Our response will be to examine the osWindow and use it to make a "Focus" event take place
  wnd.m_osWindow->SetFocus();
  m_exposeView->CloseView();
}
