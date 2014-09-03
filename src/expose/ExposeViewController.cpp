#include "stdafx.h"
#include "ExposeViewController.h"
#include "ExposeView.h"


ExposeViewController::ExposeViewController() {
  
}

ExposeViewController::~ExposeViewController() {
  
}

void ExposeViewController::OnCreate(OSWindow& window) {
  std::shared_ptr<OSWindow> windowPtr = window.shared_from_this();
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

void ExposeViewController::onWindowSelected(ExposeViewWindow& wnd) {
  // Our response will be to examine the osWindow and use it to make a "Focus" event take place
  wnd.m_osWindow->SetFocus();
}
