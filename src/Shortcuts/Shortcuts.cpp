#include "stdafx.h"
#include "Shortcuts.h"
#include "NativeUI.h"
#include "ErrorDialogs.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "expose/ExposeViewAccessManager.h"
#include "interaction/FrameFragmenter.h"
#include "osinterface/AudioVolumeInterface.h"
#include "osinterface/HtmlPageLauncher.h"
#include "osinterface/LeapInput.h"
#include "osinterface/KeepRenderWindowFullScreen.h"
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
  AutoRequired<AutoLaunch> autoLaunch;

  autoLaunch->SetFriendlyName("Leap Motion Shortcuts");

  try {
    AutoCreateContextT<ShortcutsContext> shortcutsCtxt;
    shortcutsCtxt->Initiate();
    CurrentContextPusher pshr(shortcutsCtxt);
    AutoRequired<HtmlPageLauncher>(); //needs to exist before the native ui so we can launch the help page on startup.
    AutoRequired<NativeUI> nativeUI;

    AutoRequired<ErrorDialogs>();
    AutoRequired<OSVirtualScreen> virtualScreen;
    AutoRequired<RenderWindow> renderWindow;
    AutoRequired<RenderEngine> render;
    AutoRequired<Shortcuts> shortcuts;
    AutoRequired<FrameFragmenter> fragmenter;
    AutoRequired<ExposeViewAccessManager> exposeView;
    AutoRequired<VolumeLevelChecker> volumeChecker;
    AutoDesired<AudioVolumeInterface>();
    AutoRequired<IWindowScroller>();
    AutoRequired<MediaInterface>();
    AutoRequired<LeapInput>();
    AutoRequired<OSWindowMonitor>();
    AutoRequired<KeepRenderWindowFullScreen>();

    // Register the tray icon at this point, we don't want to do it earlier because
    // a lot of stuff is happening during setup that might prevent us from being
    // responsive.
    nativeUI->ShowUI();
    auto teardown = MakeAtExit([&nativeUI] {nativeUI->DestroyUI(); });

    renderWindow->SetVSync(false);
    renderWindow->SetTransparent(true);
    renderWindow->SetVisible(true);

    // Handoff to the main loop:
    shortcuts->Main();
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

Shortcuts::Shortcuts(void)
{
}

Shortcuts::~Shortcuts(void) {}

void Shortcuts::Main(void) {
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

void Shortcuts::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}
