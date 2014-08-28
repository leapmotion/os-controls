#include "stdafx.h"
#include "ExposeViewWindow.h"
#include "osinterface/OSWindow.h"

ExposeViewWindow::ExposeViewWindow(OSWindow& osWindow):
  m_osWindow(osWindow.shared_from_this())
{

}

ExposeViewWindow::~ExposeViewWindow(void) {}
