#include "stdafx.h"
#include "ExposeTestHarness.h"
#include "TestStateMachine.h"
#include "ExposeView.h"
#include "ExposeViewController.h"
#include "graphics/RenderEngine.h"
#include "osinterface/LeapInput.h"
#include "osinterface/KeepRenderWindowFullScreen.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/OSWindowMonitor.h"
#include "uievents/Updatable.h"
#include "utility/PlatformInitializer.h"

int main(int argc, const char* argv[]) {
  // Initialize our OS-specific dependencies
  PlatformInitializer init;

  // Begin processing
  AutoCurrentContext()->Initiate();

  // Make our stuff and start processing:
  AutoRequired<OSVirtualScreen> virtualScreen;
  AutoRequired<RenderWindow> renderWindow;
  AutoRequired<RenderEngine> renderEngine;
  AutoRequired<KeepRenderWindowFullScreen>();
  AutoRequired<LeapInput> leapInput;

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
  renderWindow->SetVSync(false);
  renderWindow->SetVisible(true);

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

  return 0;
}
