#include "stdafx.h"
#include "oscontrol.h"
#include "LeapInput.h"
#include "MainWindow.h"
#include "SdlInitializer.h"
#include <autowiring/autowiring.h>
#include <SDL.h>

int main(int argc, char **argv)
{
  try {
    OsControl().Main();
  }
  catch(...) {
    return -1;
  }

  return 0;
}

OsControl::OsControl(void) :
  m_mw("")
{}

void OsControl::Main(void) {
  AutoCurrentContext()->Initiate();
  m_mw->DelegatedMain();
}