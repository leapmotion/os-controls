#include "stdafx.h"
#include "NativeUI.h"
#include "oscontrols.h"

void NativeUI::OnQuit() {
  for(auto ctxt : ContextEnumeratorT<OsControlContext>(CoreContext::CurrentContext())) {
    ctxt->SignalShutdown(false);
  }
}

void NativeUI::OnConfigUiVisible() {
}

void NativeUI::OnConfigUiHidden(bool) {
}
