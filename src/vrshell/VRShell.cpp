#include "stdafx.h"
#include "VRShell.h"

#include "graphics/RenderEngine.h"
#include "OculusVR.h"
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

  try {
    AutoCreateContextT<VRShellContext> shellCtxt;
    shellCtxt->Initiate();
    CurrentContextPusher pshr(shellCtxt);

    AutoRequired<RenderWindow> renderWindow;
    AutoRequired<VRShell> shell;
    AutoRequired<OSVirtualScreen> virtualScreen;
    AutoRequired<RenderEngine> render;
    AutoRequired<LeapInput> input;
    AutoRequired<CompositionEngine> composition;

    AutoRequired<LeapImagePassthrough>();

    AutoConstruct<OculusVR> hmdInterface;
    hmdInterface->SetWindow(renderWindow->GetSystemHandle());
    hmdInterface->Init();

    renderWindow->SetVSync(false);
    renderWindow->SetTransparent(true);

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
  Autowired<RenderWindow> renderWindow;
  AutoFired<Updatable> upd;

  // Dispatch events until told to quit:
  auto then = std::chrono::steady_clock::now();
  for(AutoCurrentContext ctxt; !ctxt->IsShutdown(); ) {
    renderWindow->ProcessEvents();
    // Broadcast update event to all interested parties:
    auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
  }
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
