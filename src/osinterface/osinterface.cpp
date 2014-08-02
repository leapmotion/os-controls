#include "stdafx.h"
#include "osinterface.h"
#include "LeapInput.h"
#include "MainWindow.h"
#include "MediaController.h"
#include "SdlInitializer.h"
#include "utility/ComInitializer.h"
#include <SDL.h>

int osinterface_main(int argc, char **argv)
{
  AutoCurrentContext ctxt;
  AutoCurrentContext()->Initiate();

  try {
    AutoRequired<OsControl> control;
    control->Main();
  }
  catch(...) {
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
  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  AutoRequired<AutoPacketFactory> factory;

  // Dispatch events until told to quit:
  while(!ShouldStop()) {
    SDL_Event ev;
    while(SDL_PollEvent(&ev))
      HandleEvent(ev);

    // Pilot a packet through the system:
    auto packet = factory->NewPacket();
  }
}

void OsControl::HandleEvent(const SDL_Event& ev) const {
  switch(ev.type) {
  case SDL_QUIT:
    AutoCurrentContext()->SignalShutdown();
    break;
  }
}

void OsControl::Filter(void) {
  try {
    throw;
  }
  catch(std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}

bool OsControl::Start(std::shared_ptr<Object> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if(m_bShouldStop)
    return true;
  m_outstanding = outstanding;
  return true;
}

void OsControl::Wait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] { return m_outstanding.get() == nullptr; });
}