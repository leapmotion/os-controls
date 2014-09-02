#include "stdafx.h"
#include "ExposeViewController.h"
#include "ExposeView.h"


ExposeViewController::ExposeViewController() {
  
}

ExposeViewController::~ExposeViewController() {
  
}

void ExposeViewController::OnCreate(const OSWindow& window) {
  std::shared_ptr<const OSWindow> windowPtr = window.shared_from_this();
  std::shared_ptr<ExposeViewWindow> newWindow = m_exposeView->NewExposeWindow(window);
  m_windows[windowPtr] = newWindow;
}

void ExposeViewController::OnDestroy(const OSWindow& window) {
  std::shared_ptr<ExposeViewWindow> viewWindow;
  std::shared_ptr<const OSWindow> windowPtr = window.shared_from_this();
  try {
    viewWindow = m_windows.at(windowPtr);
    m_exposeView->RemoveExposeWindow(viewWindow);
  }
  catch(std::out_of_range) {
    //Window not availible. Nothing to be done.
    return;
  }
}

void ExposeViewController::onSelectionMade(std::shared_ptr<const OSWindow> window) {
  //Confirm this is a window we know about...just in case the view has an old winodw.
  try {
    m_windows.at(window);
  }
  catch(std::out_of_range) {
    //Not a window we know about
    return;
  }
  /*
  if( !window->GetFocus() ) {
    
  }*/
}

bool ExposeViewController::windowExists(uint64_t uniqueId) {
  bool retVal = false;
  return retVal;
}