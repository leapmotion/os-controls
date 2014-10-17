#include "stdafx.h"
#include "VRShell.h"

#include "graphics/RenderEngine.h"
#include "hmdinterface/OculusRift/Context.h"
#include "hmdinterface/OculusRift/Device.h"
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
  AutoConstruct<OculusRift::Context> oculus_rift_context;
  oculus_rift_context->Initialize();
  assert(oculus_rift_context->IsInitialized() && "TODO: handle error the real way");

  // Create the OculusRift::Device (per-device initialization/shutdown)
  AutoConstruct<OculusRift::Device> oculus_rift_device;
  // NOTE: This SetWindow nonsense is going to be abstracted to be one parameter in a
  // DeviceInitializationParameters interface in Leap::Hmd.
  oculus_rift_device->SetWindow(renderWindow->GetSystemHandle());
  oculus_rift_device->Initialize(*static_cast<Hmd::IContext *>(oculus_rift_context));
  assert(oculus_rift_device->IsInitialized() && "TODO: handle error the real way");

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

  oculus_rift_device->Shutdown();
  oculus_rift_context->Shutdown();

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
