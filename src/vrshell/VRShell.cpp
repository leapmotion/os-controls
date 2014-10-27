#include "stdafx.h"
#include "VRShell.h"

#include "AROverlay.h"
#include "LeapImagePassthrough.h"
#include "graphics/RenderEngine.h"
#include "hmdinterface/HmdFactory.h"
#include "hmdinterface/IDevice.h"
#include "hmdinterface/IDeviceConfiguration.h"
#include "interaction/FrameFragmenter.h"
#include "interaction/SystemWipeRecognizer.h"
#include "osinterface/LeapInput.h"
#include "osinterface/RenderWindow.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/CompositionEngine.h"
#include "utility/PlatformInitializer.h"
#include <autowiring/AutoNetServer.h>
#include <iostream>

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

  AutoRequired<RenderWindow> renderEngineWindow;
  renderEngineWindow->SetActive();
  AutoRequired<RenderEngine>();
  AutoRequired<OSVirtualScreen>();
  AutoRequired<RawFrameFragmenter> fragmenter;
  AutoRequired<AROverlay> overlay;

  // Create the OculusRift::Context (non-device initialization/shutdown)
  // This really needs to be done in a factory - we have no business knowing
  // about the underlying implementation.  Doing so is counter to the entire point of
  // having an abstract interface in the first place.
  AutoRequired<Hmd::HmdFactory> hmdFactory;

  // Create the OculusRift::Device (per-device initialization/shutdown)
  AutoRequired<Hmd::IDevice> hmdDevice;

  hmdDevice->SetWindow(renderEngineWindow->GetSystemHandle());
  hmdDevice->Initialize();
  
  const auto &hmdConfiguration = hmdDevice->Configuration();

  renderEngineWindow->SetRect(OSRect(hmdConfiguration.WindowPositionX(), hmdConfiguration.WindowPositionY(), 
                             hmdConfiguration.DisplayWidth(), hmdConfiguration.DisplayHeight()));
  renderEngineWindow->SetVSync(false);
  renderEngineWindow->SetTransparent(false);
  renderEngineWindow->SetVisible(true);

  overlay->SetSourceWindow(*renderEngineWindow);

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
    renderEngineWindow->ProcessEvents();

    // Handle autowiring events:
    DispatchAllEvents();
    // Broadcast update event to all interested parties:
    const auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
  }

  hmdDevice->Shutdown();

  renderEngineWindow->SetVisible(false);
}

void VRShell::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
