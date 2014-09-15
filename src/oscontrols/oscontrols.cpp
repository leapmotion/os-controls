#include "stdafx.h"
#include "oscontrols.h"
#include "NativeUI.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "expose/ExposeViewAccessManager.h"
#include "interaction/FrameFragmenter.h"
#include "osinterface/AudioVolumeInterface.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
#include "osinterface/MediaInterface.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/WindowScroller.h"
#include "osinterface/VolumeLevelChecker.h"
#include "osinterface/OSWindowMonitor.h"
#include "uievents/SystemMultimediaEventListener.h"
#include "utility/Config.h"
#include "utility/FileMonitor.h"
#include "utility/NativeWindow.h"
#include "utility/PlatformInitializer.h"
#include <autowiring/AutoNetServer.h>

int main(int argc, char **argv)
{
  PlatformInitializer init;
  AutoCurrentContext ctxt;
  AutoRequired<Config> config;
  AutoRequired<FileMonitor> fileMonitor;


  ctxt->Initiate();

  try {
    AutoCreateContextT<OsControlContext> osCtxt;
    osCtxt->Initiate();
    CurrentContextPusher pshr(osCtxt);
    AutoRequired<NativeUI> nativeUI;

    // Register the tray icon early in the process, before we spend a bunch of time doing everything else
    nativeUI->ShowUI();
    auto teardown = MakeAtExit([&nativeUI] {nativeUI->DestroyUI(); });

    AutoRequired<RenderEngine> render;
    AutoRequired<OSVirtualScreen> virtualScreen;
    AutoRequired<OsControl> control;
    AutoRequired<FrameFragmenter> fragmenter;
    AutoConstruct<sf::ContextSettings> contextSettings(0, 0, 16);
    AutoRequired<ExposeViewAccessManager> exposeView;
    AutoRequired<VolumeLevelChecker> volumeChecker;
    AutoDesired<AudioVolumeInterface>();
    AutoRequired<IWindowScroller>();
    AutoRequired<MediaInterface>();
    AutoRequired<LeapInput>();
    AutoRequired<MakesRenderWindowFullScreen>();
    AutoRequired<OSWindowMonitor>();
    AutoConstruct<sf::RenderWindow> mw(
      sf::VideoMode(1, 1),
      "Leap Hand Control", sf::Style::None,
      *contextSettings
    );

    // Run as fast as possible:
    mw->setFramerateLimit(0);
    mw->setVerticalSyncEnabled(true);

    // Handoff to the main loop:
    control->Main();
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

OsControl::OsControl(void)
{
}

OsControl::~OsControl(void) {}

void OsControl::Main(void) {
  AutoFired<Updatable> upd;

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

void OsControl::HandleEvent(const sf::Event& ev) const {
  switch (ev.type) {
  case sf::Event::Closed:
    AutoCurrentContext()->SignalShutdown();
    break;
  default:
    break;
  }
}

void OsControl::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
