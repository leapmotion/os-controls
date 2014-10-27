#include "stdafx.h"
#include "VRShell.h"

#include "graphics/RenderEngine.h"
#include "hmdinterface/HmdFactory.h"
#include "hmdinterface/IDevice.h"
#include "osinterface/LeapInput.h"
#include "utility/PlatformInitializer.h"
#include "LeapImagePassthrough.h"
#include "osinterface/RenderWindow.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/CompositionEngine.h"
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

  AutoRequired<RenderWindow> renderWindow;
  AutoRequired<OSVirtualScreen>();
  AutoRequired<RenderEngine>();
  AutoRequired<CompositionEngine>();

  // Create the OculusRift::Context (non-device initialization/shutdown)
  // This really needs to be done in a factory - we have no business knowing
  // about the underlying implementation.  Doing so is counter to the entire point of
  // having an abstract interface in the first place.
  AutoRequired<Hmd::HmdFactory> hmdFactory;

  // Create the OculusRift::Device (per-device initialization/shutdown)
  //Todo - make this have an autowiring compatible interface.
  std::shared_ptr<Hmd::IDevice> hmdDevice(hmdFactory->CreateDevice());
  // NOTE: This SetWindow nonsense is going to be abstracted to be one parameter in a
  // DeviceInitializationParameters interface in Leap::Hmd.
  hmdDevice->SetWindow(renderWindow->GetSystemHandle());
  hmdDevice->Initialize();
  assert(hmdDevice->IsInitialized() && "TODO: handle error the real way");

  renderWindow->SetVSync(false);
  renderWindow->SetSize({640, 480});
  renderWindow->SetTransparent(false);
  renderWindow->SetVisible(true);

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
    renderWindow->ProcessEvents();
    // Handle autowiring events:
    DispatchAllEvents();
    // Broadcast update event to all interested parties:
    auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
  }

  hmdDevice->Shutdown();

  renderWindow->SetVisible(false);
}

void VRShell::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
