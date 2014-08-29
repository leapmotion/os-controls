#include "stdafx.h"
#include "oscontrols.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "expose/ExposeViewAccessManager.h"
#include "interaction/FrameFragmenter.h"
#include "osinterface/AudioVolumeInterface.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
#include "osinterface/MediaInterface.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/VolumeLevelChecker.h"
#include "uievents/SystemMultimediaEventListener.h"
#include "utility/NativeWindow.h"
#include "utility/PlatformInitializer.h"

int main(int argc, char **argv)
{
  PlatformInitializer init;
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  try {
    AutoCreateContextT<OsControlContext> osCtxt;
    osCtxt->Initiate();

    CurrentContextPusher pshr(osCtxt);
    AutoRequired<RenderEngine> render;
    AutoRequired<OSVirtualScreen> virtualScreen;
    AutoRequired<OsControl> control;
    AutoRequired<FrameFragmenter> fragmenter;
    AutoConstruct<sf::ContextSettings> contextSettings(0, 0, 16);
    AutoRequired<ExposeViewAccessManager> exposeView;
    AutoRequired<VolumeLevelChecker> volumeChecker;
    AutoDesired<AudioVolumeInterface>();
    AutoRequired<MediaInterface>();
    AutoRequired<LeapInput>();
    AutoRequired<MakesRenderWindowFullScreen>();
    AutoConstruct<sf::RenderWindow> m_mw(
      sf::VideoMode(
        (int) virtualScreen->PrimaryScreen().Width(),
        (int) virtualScreen->PrimaryScreen().Height()
      ),
      "Leap Os Control", sf::Style::None,
      *contextSettings
    );

    // Handoff to the main loop:
    control->Main();
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

OsControl::OsControl(void) :
  m_bShouldStop(false),
  m_bRunning(false)
{
}

OsControl::~OsControl(void) {}

void OsControl::Main(void) {
  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  AutoFired<Updatable> upd;

  // Dispatch events until told to quit:
  auto then = std::chrono::steady_clock::now();
  while(!ShouldStop()) {
    // Handle all events:
    for(sf::Event evt; m_mw->pollEvent(evt);)
      HandleEvent(evt);

    // Broadcast update event to all interested parties:
    auto now = std::chrono::steady_clock::now();
    upd(&Updatable::Tick)(now - then);
    then = now;
  }
}

void OsControl::HandleEvent(const sf::Event& ev) const {
  switch (ev.type) {
  case sf::Event::Closed:
    m_mw->close();
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

bool OsControl::Start(std::shared_ptr<Object> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if (m_bShouldStop)
    return true;
  m_outstanding = outstanding;
  return true;
}

void OsControl::Wait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] { return m_outstanding.get() == nullptr; });
}
