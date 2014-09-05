#include "stdafx.h"
#include "ExposeTestHarness.h"
#include "TestStateMachine.h"
#include "ExposeView.h"
#include "ExposeViewController.h"
#include "graphics/RenderEngine.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/OSWindowMonitor.h"
#include "osinterface/WindowRenderer.h"
#include "uievents/Updatable.h"
#include "utility/PlatformInitializer.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowStyle.hpp>

int main(int argc, const char* argv[]) {
  // Initialize our OS-specific dependencies
  PlatformInitializer init;
  
  // Begin processing
  AutoCurrentContext()->Initiate();

  // Make our stuff and start processing:
  AutoRequired<RenderEngine> renderEngine;
  AutoRequired<OSVirtualScreen> virtualScreen;
  AutoRequired<WindowRenderer>();
  AutoConstruct<sf::ContextSettings> contextSettings(0, 0, 16);
  AutoRequired<LeapInput> leapInput;
  AutoRequired<MakesRenderWindowFullScreen>();
  AutoConstruct<sf::RenderWindow> mw(
    sf::VideoMode(
      (int) virtualScreen->PrimaryScreen().Width(),
      (int) virtualScreen->PrimaryScreen().Height()
    ),
    "Expose Mode Test Harness", sf::Style::None,
    *contextSettings
  );

  // Create all of our expose stuff after everything else is set up
  AutoRequired<ExposeView> view;
  AutoRequired<ExposeViewController>();

  // We always want the view to be 100% visible
  view->StartView();

  // We will also need to monitor the state of all screen windows:
  AutoRequired<OSWindowMonitor>();

  // Need to make a state machine
  AutoRequired<TestStateMachine> tss;

  // Primary dispatch loop:
  mw->setFramerateLimit(0);
  mw->setVerticalSyncEnabled(true);

  AutoFired<Updatable> upd;
  for(auto then = std::chrono::steady_clock::now();;) {
    // Event dispatch in sf:
    for(sf::Event evt; mw->pollEvent(evt);)
      switch(evt.type) {
        case sf::Event::Closed:
          mw->close();
          AutoCurrentContext()->SignalShutdown(true);
          return 0;
        default:
          break;
    }

    // Broadcast update event to all interested parties:
    auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
  }

  return 0;
}
