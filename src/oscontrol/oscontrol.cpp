#include "stdafx.h"
#include "oscontrol.h"
#include "LeapInput.h"
#include "MainWindow.h"
#include "SdlInitializer.h"
#include <SDL.h>

int main(int argc, char **argv)
{
  AutoCurrentContext ctxt;

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
  m_mw("")
{}

void OsControl::Main(void) {
  AutoCurrentContext()->Initiate();
  m_mw->DelegatedMain();
}

void OsControl::Filter(void) {
  try {
    throw;
  }
  catch(std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}