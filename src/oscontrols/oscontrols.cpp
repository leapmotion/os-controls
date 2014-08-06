#include "stdafx.h"
#include "interaction/GestureTriggerManifest.h"
#include "oscontrols.h"
#include "osinterface/AudioVolumeController.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MainWindow.h"
#include "osinterface/MediaController.h"
#include "osinterface/SdlInitializer.h"
#include "utility/ComInitializer.h"
#include <SDL.h>

class Drawable;

struct Scene {
  std::list<Drawable*> m_drawables;
};

int main(int argc, char **argv)
{
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  try {
    AutoRequired<OsControl> control;
    control->Main();
  }
  catch (...) {
    return -1;
  }

  ctxt->SignalShutdown(true);
  return 0;
}

OsControl::OsControl(void) :
m_mw(""),
m_bShouldStop(false),
m_bRunning(false)
{}

void OsControl::Main(void) {
  ComInitializer initCom;
  GestureTriggerManifest manifest;

  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  AutoRequired<AutoPacketFactory> factory;

  // Dispatch events until told to quit:
  while (!ShouldStop()) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
      HandleEvent(ev);

    // Pilot a packet through the system: 
    auto packet = factory->NewPacket();

    Scene scene;
    packet->DecorateImmediate(scene);
    for(auto drawable : scene.m_drawables)
      drawable->Draw();
  }
}

void OsControl::HandleEvent(const SDL_Event& ev) const {
  switch (ev.type) {
  case SDL_QUIT:
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