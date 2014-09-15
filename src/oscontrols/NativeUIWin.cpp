#include "NativeUI.h"
#include "NativeUIWin.h"

using namespace oscontrols;

void NativeUI::ShowUI() {
  NativeUIWin::AddTrayIcon(*this);
}

void NativeUI::DestroyUI(void) {
  Application::Exit();
}
