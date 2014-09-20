#include "stdafx.h"
#include "Shortcuts.h"
#include "NativeUI.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "expose/ExposeViewAccessManager.h"
#include "interaction/FrameFragmenter.h"
#include "osinterface/AudioVolumeInterface.h"
#include "osinterface/HtmlPageLauncher.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
#include "osinterface/MediaInterface.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/OSWindowMonitor.h"
#include "osinterface/VolumeLevelChecker.h"
#include "osinterface/WindowScroller.h"
#include "uievents/SystemMultimediaEventListener.h"
#include "utility/AutoLaunch.h"
#include "utility/Config.h"
#include "utility/FileMonitor.h"
#include "utility/NativeWindow.h"
#include "utility/PlatformInitializer.h"
#include <autowiring/AutoNetServer.h>

int main(int argc, char **argv)
{
  PlatformInitializer init;
  AutoCurrentContext ctxt;
  
  ctxt->Initiate();
  AutoRequired<Config> config; //do this just after the native ui is created so it gets the OnSettingChanged events.
  AutoConstruct<AutoLaunch> autoLaunch("Leap Motion Shortcuts");

  try {
    AutoCreateContextT<ShortcutsContext> shortcutsCtxt;
    shortcutsCtxt->Initiate();
    CurrentContextPusher pshr(shortcutsCtxt);
    AutoRequired<HtmlPageLauncher>(); //needs to exist before the native ui so we can launch the help page on startup.
    AutoRequired<NativeUI> nativeUI;

    // Register the tray icon early in the process, before we spend a bunch of time doing everything else
    nativeUI->ShowUI();
    auto teardown = MakeAtExit([&nativeUI] {nativeUI->DestroyUI(); });

    AutoRequired<RenderEngine> render;
    AutoRequired<OSVirtualScreen> virtualScreen;
    AutoRequired<Shortcuts> shortcuts;
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
      "Shortcuts", sf::Style::None,
      *contextSettings
    );

    // Run as fast as possible:
    mw->setFramerateLimit(120);
    mw->setVerticalSyncEnabled(false);

    // Handoff to the main loop:
    shortcuts->Main();
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

Shortcuts::Shortcuts(void)
{
}

Shortcuts::~Shortcuts(void) {}

void Shortcuts::Main(void) {
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

void Shortcuts::HandleEvent(const sf::Event& ev) const {
  switch (ev.type) {
  case sf::Event::Closed:
    AutoCurrentContext()->SignalShutdown();
    break;
  default:
    break;
  }
}

void Shortcuts::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
