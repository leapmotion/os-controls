#include "stdafx.h"
#include "oscontrols.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "interaction/FrameFragmenter.h"
#include "osinterface/AudioVolumeInterface.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MediaInterface.h"
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
    AutoRequired<leap::VirtualScreen> virtualScreen;
    AutoRequired<OsControl> control;
    AutoRequired<FrameFragmenter> fragmenter;
    control->Main();
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

OsControl::OsControl(void) :
  m_contextSettings(0, 0, 16),
  m_mw(sf::VideoMode((int)m_virtualScreen->PrimaryScreen().Width(),
                     (int)m_virtualScreen->PrimaryScreen().Height()),
                     "Leap Os Control", sf::Style::None,
                     m_contextSettings),
  m_bShouldStop(false),
  m_bRunning(false),
  m_desktopChanged{1} // Also perform an adjust in the main loop
{
  AutoRequired<VolumeLevelChecker>();
  AdjustDesktopWindow();
}

void OsControl::AdjustDesktopWindow(void) {
  m_mw->setVisible(false);
  const sf::Vector2i olPosition = m_mw->getPosition();
  const sf::Vector2u oldSize = m_mw->getSize();

  const auto bounds = m_virtualScreen->PrimaryScreen().Bounds();
  const sf::Vector2i newPosition = { static_cast<int32_t>(bounds.origin.x),
                                     static_cast<int32_t>(bounds.origin.y) };
  const sf::Vector2u newSize = { static_cast<uint32_t>(bounds.size.width),
                                 static_cast<uint32_t>(bounds.size.height) };

  if (oldSize != newSize) {
    m_mw->create(sf::VideoMode(newSize.x, newSize.y), "Leap Os Control", sf::Style::None, m_contextSettings);
  }
  m_mw->setPosition(newPosition);
  const auto handle = m_mw->getSystemHandle();
  NativeWindow::MakeTransparent(handle);
  NativeWindow::MakeAlwaysOnTop(handle);
  NativeWindow::AllowInput(handle, false);
  m_mw->setVisible(true);
}

void OsControl::Main(void) {
  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  auto then = std::chrono::steady_clock::now();

  m_mw->setFramerateLimit(0);
  m_mw->setVerticalSyncEnabled(true);
  AutoFired<Updatable> upd;

  // Dispatch events until told to quit:
  while (!ShouldStop()) {
    // Our chance to position and possibly recreate the window if the desktop has changed
    if (m_desktopChanged) {
      --m_desktopChanged; // Do one at a time
      AdjustDesktopWindow();
    }

    for(sf::Event evt; m_mw->pollEvent(evt);)
      HandleEvent(evt);

    // Determine how long it has been since we were last here
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeDelta = now - then;
    then = now;

    // Broadcast update event:
    upd(&Updatable::Tick)(timeDelta);

    // Call the actual render behavior:
    m_render->Render(m_mw, timeDelta);
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
