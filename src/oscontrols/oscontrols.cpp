#include "stdafx.h"
#include "interaction/GestureTriggerManifest.h"
#include "oscontrols.h"
#include "osinterface/AudioVolumeController.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MediaController.h"
#include "utility/ComInitializer.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

struct OSControlContext {};

class Drawable;

struct Scene {
  std::list<Drawable*> m_drawables;
};

int main(int argc, char **argv)
{
  ComInitializer initCom;
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  try {
    AutoCreateContextT<OSControlContext> osCtxt;
    CurrentContextPusher pshr(osCtxt);
    AutoRequired<OsControl> control;
    osCtxt->Initiate();
    control->Main();
  }
  catch (...) {}

  ctxt->SignalShutdown(true);
  return 0;
}

OsControl::OsControl(void) :
m_mw(sf::VideoMode(800,600),"Leap Os Control"),
m_bShouldStop(false),
m_bRunning(false)
{
}

void OsControl::Main(void) {

  GestureTriggerManifest manifest;

  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  AutoRequired<AutoPacketFactory> factory;

  // Dispatch events until told to quit:
  while (!ShouldStop()) {
    sf::Event event;
    while (m_mw->pollEvent(event)){
      HandleEvent(event);
    }

    // Pilot a packet through the system:
    auto packet = factory->NewPacket();

    Scene scene;
    packet->DecorateImmediate(scene);
    for(auto drawable : scene.m_drawables)
      drawable->Draw();
  }
}

void OsControl::HandleEvent(const sf::Event& ev) const {
  switch (ev.type) {
  case sf::Event::Closed:
    m_mw->close();
    AutoCurrentContext()->SignalShutdown();
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
