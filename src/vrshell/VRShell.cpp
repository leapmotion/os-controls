#include "stdafx.h"
#include "VRShell.h"

#include "graphics/RenderEngine.h"
#include "hmdinterface/OculusRift/RiftContext.h"
#include "hmdinterface/OculusRift/RiftDevice.h"
#include "osinterface/LeapInput.h"
#include "utility/PlatformInitializer.h"
#include "LeapImagePassthrough.h"
#include "osinterface/RenderWindow.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/CompositionEngine.h"
#include <autowiring/AutoNetServer.h>
#include <iostream>

#include <dwmapi.h>

int main(int argc, char **argv)
{
  PlatformInitializer init;
  AutoCurrentContext ctxt;

  ctxt->Initiate();
  AutoRequired<VRShell> shell;

  try {
    // Handoff to the main loop:
    shell->Main();
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

#if _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  return main(__argc, __argv);
}
#endif

VRShell::VRShell(void)
{
}

VRShell::~VRShell(void) {}

void VRShell::Main(void) {
  AutoCreateContextT<VRShellContext> shellCtxt;
  shellCtxt->Initiate();
  CurrentContextPusher pshr(shellCtxt);

  AutoRequired<RenderWindow> graphicsWindow;
  std::unique_ptr<RenderWindow> copyWindow(RenderWindow::New());
  std::unique_ptr<RenderWindow> mainWindow(RenderWindow::New());

  AutoRequired<OSVirtualScreen> osScreens;
  AutoRequired<RenderEngine>();
  AutoRequired<CompositionEngine> compositionEngine;

  // Create the OculusRift::Context (non-device initialization/shutdown)
  // This really needs to be done in a factory - we have no business knowing
  // about the underlying implementation.  Doing so is counter to the entire point of
  // having an abstract interface in the first place.
  AutoRequired<OculusRift::Context> hmdContext;
  AutoRequired<OculusRift::Device> hmdDevice;
  mainWindow->SetActive(true);

  hmdContext->Initialize();
  hmdDevice->SetWindow(mainWindow->GetSystemHandle());
  hmdDevice->Initialize(*static_cast<Hmd::IContext *>(hmdContext));

  auto cfg = hmdDevice->Configuration();

  HWND oldWindow = WindowFromPoint({ cfg.WindowPositionX() + (cfg.DisplayWidth() / 2), cfg.WindowPositionY() + (cfg.DisplayHeight() / 2) });

  graphicsWindow->SetRect({ 0, 0, 640, 480 });
  graphicsWindow->SetVisible(true);

  RECT winRect;
  ::GetWindowRect(oldWindow, &winRect);
  float width = winRect.right - winRect.left;
  float height = winRect.bottom - winRect.top;
  copyWindow->SetRect({ 0.f, 500.f, width, height });
  copyWindow->SetVisible(true);


  HTHUMBNAIL thumbnail;
  ::DwmRegisterThumbnail(copyWindow->GetSystemHandle(), oldWindow, &thumbnail);

  DWM_THUMBNAIL_PROPERTIES properties;
  RECT dest = { 0, 0, width, height };
  properties.fSourceClientAreaOnly = FALSE;
  properties.fVisible = TRUE;
  properties.opacity = (255 * 70) / 100;
  properties.rcDestination = dest;
  properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;

  mainWindow->SetRect(OSRect(cfg.WindowPositionX(), cfg.WindowPositionY(), cfg.DisplayWidth(), cfg.DisplayHeight()));
  mainWindow->SetVSync(false);
  mainWindow->SetTransparent(false);
  mainWindow->SetVisible(true);

  copyWindow->SetCloaked();
  graphicsWindow->SetCloaked();

  std::unique_ptr<ComposedDisplay> compDisplay(compositionEngine->CreateDisplay(mainWindow->GetSystemHandle()));
  std::unique_ptr<ComposedView> oldWindowView(compositionEngine->CreateView());
  std::unique_ptr<ComposedView> leapImageView(compositionEngine->CreateView());
  std::unique_ptr<ComposedView> totalView(compositionEngine->CreateView());

  oldWindowView->SetContent(copyWindow->GetSystemHandle());
  leapImageView->SetContent(graphicsWindow->GetSystemHandle());
  leapImageView->SetScale(0, 0, width / graphicsWindow->GetSize().width, height / graphicsWindow->GetSize().height);
  totalView->AddChild(oldWindowView.get());
  totalView->AddChild(leapImageView.get());

  compDisplay->SetView(totalView.get());

  compositionEngine->CommitChanges();

  // Defer starting any Leap handling until the window is ready
  *this += [this] {
    AutoRequired<LeapInput> leap;
    AutoRequired<LeapImagePassthrough>();
    leap->AddPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
  };

  AutoFired<Updatable> upd;

  // Dispatch events until told to quit:
  
  auto then = std::chrono::steady_clock::now();
  for(AutoCurrentContext ctxt; !ctxt->IsShutdown(); ) {
    // Handle OS events:
    mainWindow->ProcessEvents();
    graphicsWindow->ProcessEvents();

    //update the thumbnail

    ::DwmUpdateThumbnailProperties(thumbnail, &properties);

    // Handle autowiring events:
    DispatchAllEvents();
    // Broadcast update event to all interested parties:
    auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
  }

  hmdDevice->Shutdown();
  hmdContext->Shutdown();

  mainWindow->SetVisible(false);
  graphicsWindow->SetVisible(false);
}

void VRShell::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
