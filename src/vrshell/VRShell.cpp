#include "stdafx.h"

#include "Leap/OculusRift/Context.h"
#include "Leap/OculusRift/Device.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
#include "utility/PlatformInitializer.h"
#include "VRShell.h"

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

    AutoRequired<VRShell> shell;
    AutoConstruct<sf::ContextSettings> contextSettings(0, 0, 16);
    AutoConstruct<sf::RenderWindow> mw(
      sf::VideoMode(1, 1),
      "VRShell", sf::Style::None,
      *contextSettings
    );
    AutoRequired<MakesRenderWindowFullScreen>();

    // Create the OculusRift::Context (non-device initialization/shutdown)
    AutoConstruct<Leap::OculusRift::Context> oculus_rift_context;
    oculus_rift_context->Initialize();
    assert(oculus_rift_context->IsInitialized() && "TODO: handle error the real way");

    // Create the OculusRift::Device (per-device initialization/shutdown)
    AutoConstruct<Leap::OculusRift::Device> oculus_rift_device;
    // NOTE: This SetWindow nonsense is going to be abstracted to be one parameter in a
    // DeviceInitializationParameters interface in Leap::Hmd.
    oculus_rift_device->SetWindow(mw->getSystemHandle());
    oculus_rift_device->Initialize(*static_cast<Leap::Hmd::Context *>(oculus_rift_context));
    assert(oculus_rift_device->IsInitialized() && "TODO: handle error the real way");

    // Run as fast as possible:
    mw->setFramerateLimit(120);
    mw->setVerticalSyncEnabled(false);

    // Handoff to the main loop:
    shell->Main();

    oculus_rift_device->Shutdown();
    oculus_rift_context->Shutdown();
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

VRShell::VRShell(void)
{
}

VRShell::~VRShell(void) {}

void VRShell::Main(void) {
  //AutoFired<Updatable> upd;

  // Dispatch events until told to quit:
  auto then = std::chrono::steady_clock::now();
  for(AutoCurrentContext ctxt; !ctxt->IsShutdown(); ) {
    // Handle all events:
    for(sf::Event evt; m_mw->pollEvent(evt);)
      HandleEvent(evt);

    // Broadcast update event to all interested parties:
    //auto now = std::chrono::steady_clock::now();
    //upd(&Updatable::Tick)(now - then);
    //then = now;
  }
  m_mw->close();
}

void VRShell::HandleEvent(const sf::Event& ev) const {
  switch (ev.type) {
  case sf::Event::Closed:
    AutoCurrentContext()->SignalShutdown();
    break;
  default:
    break;
  }
}

void VRShell::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
