#include "stdafx.h"
#include "DwmThumbnailWindow.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <dwmapi.h>

DwmThumbnailWindow::DwmThumbnailWindow(HWND hwnd):
  hwnd(hwnd),
  thumbnailID(RegisterThumbnail())
{
}

DwmThumbnailWindow::~DwmThumbnailWindow(void) {
}

bool DwmThumbnailWindow::Update(void) const {
  // Obtain the remote size
  RECT rect;
  if(!GetWindowRect(hwnd, &rect))
    // Other window must be gone, nothing we can do, get out of here
    return false;

  // Now we can update ourselves:
  DWM_THUMBNAIL_PROPERTIES props = {};
  props.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTSOURCE | DWM_TNP_RECTDESTINATION;
  
  props.rcSource.left = 0;
  props.rcSource.right = rect.right - rect.left;
  props.rcSource.top = 0;
  props.rcSource.bottom = rect.bottom - rect.top;

  rect.left = 680;
  rect.top = 700;
  rect.bottom = 680 + 100;
  rect.right = 700 + 100;
  props.rcDestination = rect;

  props.opacity = 25;

  props.fVisible = true;
  HRESULT hr = DwmUpdateThumbnailProperties(thumbnailID, &props);
  return SUCCEEDED(hr);
}

HTHUMBNAIL DwmThumbnailWindow::RegisterThumbnail(void) {
  Autowired<sf::RenderWindow> mw;
  HWND hwndProxy = mw->getSystemHandle();

  HTHUMBNAIL retVal;
  HRESULT hr = DwmRegisterThumbnail(hwndProxy, hwnd, &retVal);
  if(FAILED(hr))
    throw std::runtime_error("Failed to register a DWM thumbnail for a foreign window");

  return retVal;
}