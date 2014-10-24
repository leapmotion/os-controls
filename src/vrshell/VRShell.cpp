#include "stdafx.h"
#include "VRShell.h"

#include "LeapImagePassthrough.h"
#include "graphics/RenderEngine.h"
#include "hmdinterface/OculusRift/RiftContext.h"
#include "hmdinterface/OculusRift/RiftDevice.h"
#include "interaction/FrameFragmenter.h"
#include "interaction/SystemWipeRecognizer.h"
#include "osinterface/LeapInput.h"
#include "osinterface/RenderWindow.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/CompositionEngine.h"
#include "utility/PlatformInitializer.h"
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

struct WipeListener{
  void AutoFilter(const SystemWipe& wipe) {
    isWiping = wipe.isWiping;
    if ( isWiping )
      lastDirection = wipe.direction;
  }
  bool isWiping = false;
  SystemWipe::Direction lastDirection = SystemWipe::Direction::DOWN;
};

void VRShell::Main(void) {
  AutoCreateContextT<VRShellContext> shellCtxt;
  shellCtxt->Initiate();
  CurrentContextPusher pshr(shellCtxt);

  AutoRequired<RenderWindow> graphicsWindow;
  std::unique_ptr<RenderWindow> mainWindow(RenderWindow::New());

  AutoRequired<OSVirtualScreen> osScreens;
  AutoRequired<RenderEngine>();
  AutoRequired<CompositionEngine> compositionEngine;
  AutoRequired<RawFrameFragmenter> fragmenter;
  AutoRequired<SystemWipeRecognizer> wipeRecognizer;
  AutoRequired<WipeListener> wipeListener;

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

  const auto &hmdConfiguration = hmdDevice->Configuration();

  graphicsWindow->SetRect({ 0, 0, 640, 480 });
  graphicsWindow->SetVisible(true);

  float screenWidth = hmdConfiguration.DisplayWidth();
  float screenHeight = hmdConfiguration.DisplayHeight();

  mainWindow->SetRect(OSRect(hmdConfiguration.WindowPositionX(), hmdConfiguration.WindowPositionY(), 
                             hmdConfiguration.DisplayWidth(), hmdConfiguration.DisplayHeight()));
  mainWindow->SetVSync(false);
  mainWindow->SetTransparent(false);
  mainWindow->SetVisible(true);

  graphicsWindow->SetCloaked();

  std::unique_ptr<ComposedDisplay> compDisplay(compositionEngine->CreateDisplay(mainWindow->GetSystemHandle()));
  std::unique_ptr<ComposedView> leapImageView(compositionEngine->CreateView());
  std::unique_ptr<ComposedView> totalView(compositionEngine->CreateView());

  leapImageView->SetContent(graphicsWindow->GetSystemHandle());
  leapImageView->SetScale(0, 0, screenWidth / graphicsWindow->GetSize().width, screenHeight / graphicsWindow->GetSize().height);
  leapImageView->SetClip(0, 0, screenWidth, 0);
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
  float offset = 0;
  bool showingLeapPassthrough = false;
  auto then = std::chrono::steady_clock::now();
  for(AutoCurrentContext ctxt; !ctxt->IsShutdown(); ) {
    // Handle OS events:
    mainWindow->ProcessEvents();
    graphicsWindow->ProcessEvents();

    // Handle autowiring events:
    this->DispatchAllEvents();

    // Broadcast update event to all interested parties:
    const auto now = std::chrono::steady_clock::now();
    const auto delta = now - then;
    then = now;

    upd(&Updatable::Tick)(delta);

    //update the thumbnail & dcomp stuff
    const double seconds = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();
    const double animDuration = 0.5;

    if (wipeListener->isWiping) {
      showingLeapPassthrough = !showingLeapPassthrough;
    }

    if (showingLeapPassthrough) {
      offset += (screenHeight / animDuration) * seconds;
      offset = std::max(0.f, std::min(screenHeight, offset));

      if (wipeListener->lastDirection == SystemWipe::Direction::DOWN) {
        leapImageView->SetClip(0, 0, screenWidth, offset);
      }
      else{
        leapImageView->SetClip(0, screenHeight - offset, screenWidth, offset);
      }
    }
    else {
      offset -= (screenHeight / animDuration) * seconds;
      offset = std::max(0.f, std::min(screenHeight, offset));

      if (wipeListener->lastDirection == SystemWipe::Direction::DOWN) {
        leapImageView->SetClip(0, screenHeight - offset, screenWidth, offset);
      }
      else {
        leapImageView->SetClip(0, 0, screenWidth, offset);
      }
    }
    
    compositionEngine->CommitChanges();
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
