#include "NativeUI.h"
#include "NativeUIWin.h"

using namespace oscontrols;

void NativeUI::ShowUI() {
  NativeUIWin::AddTrayIcon(*this);
}

void NativeUI::DestroyUI(void) {
  Application::Exit();
}

void NativeUI::OnQuit() {
  for (auto ctxt : ContextEnumeratorT<OsControlContext>(CoreContext::CurrentContext())) {
    ctxt->SignalShutdown(false);
  }
}

void NativeUI::OnConfigUiVisible() {
}

void NativeUI::OnConfigUiHidden(bool) {
}
