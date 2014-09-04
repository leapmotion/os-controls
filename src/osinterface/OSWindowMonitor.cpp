#include "stdafx.h"
#include "OSWindowMonitor.h"

OSWindowMonitor::OSWindowMonitor(void):
  ContextMember("OSWindowMonitor")
{
}

OSWindowMonitor::~OSWindowMonitor(void)
{
}

void OSWindowMonitor::Tick(std::chrono::duration<double> deltaT) {
  Scan();
}
