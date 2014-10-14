#include "stdafx.h"
#include "VRShell.h"

#include "graphics/RenderEngine.h"
#include "OculusVR.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
#include "utility/PlatformInitializer.h"
#include "LeapImagePassthrough.h"
#include "osinterface/CompositionEngine.h"
#include <autowiring/AutoNetServer.h>
#include <iostream>
#include <SFML/Window.hpp>

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
    AutoRequired<OSVirtualScreen> virtualScreen;
    AutoRequired<RenderEngine> render;
    AutoRequired<LeapInput> input;
    AutoRequired<CompositionEngine> composition;
    
    AutoConstruct<sf::ContextSettings> contextSettings(0, 0, 16);
    AutoConstruct<sf::RenderWindow> mw(
      sf::VideoMode(640, 480),
      "VRShell", sf::Style::Titlebar,
      *contextSettings
    );

    AutoRequired<LeapImagePassthrough>();

    //AutoRequired<MakesRenderWindowFullScreen>();
    AutoConstruct<OculusVR> hmdInterface;
    hmdInterface->SetWindow(mw->getSystemHandle());
    hmdInterface->Init();

    // Run as fast as possible:
    mw->setFramerateLimit(120);
    mw->setVerticalSyncEnabled(false);

    // Handoff to the main loop:
    shell->Main();
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
  AutoFired<Updatable> upd;

  sf::Window window;
  Autowired<sf::ContextSettings> contextSettings;
  window.create(sf::VideoMode::VideoMode(200, 200), "secondary window");

  Autowired<CompositionEngine> engine;
  auto display = engine->CreateDisplay(window.getSystemHandle());
  auto view = engine->CreateView();

  Autowired<sf::RenderWindow> mainWindow;
  view->SetContent(mainWindow->getSystemHandle());
  view->SetScale(0.f, 0.f, .2f, .2f);
  view->SetOffset(20, 20);
  view->SetRotation(40, 40, 30);
  display->SetView(view);
  
  engine->CommitChanges();

  // Dispatch events until told to quit:
  auto then = std::chrono::steady_clock::now();
  for(AutoCurrentContext ctxt; !ctxt->IsShutdown(); ) {
    // Handle all events:
    for(sf::Event evt; m_mw->pollEvent(evt);)
      HandleEvent(evt);

    // Broadcast update event to all interested parties:
    auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
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
