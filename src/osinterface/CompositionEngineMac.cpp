#include "CompositionEngineMac.h"


CompositionEngine* CompositionEngine::New() { return new CompositionEngineMac; }

CompositionEngineMac::CompositionEngineMac()
{
}

ComposedView* CompositionEngineMac::CreateView() {
  return new ComposedViewMac;
}

ComposedDisplay* CompositionEngineMac::CreateDisplay(WindowHandle handle) {
  return new ComposedDisplayMac;
}

void CompositionEngineMac::CommitChanges() {

}

bool CompositionEngineMac::CommitRequired() const {
  return false;
}

