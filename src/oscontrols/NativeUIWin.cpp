#include "NativeUI.h"
#include "NativeUIWin.h"

using namespace oscontrols;

void ShowUI(const NativeCallbacks& callbacks) {
  NativeUIWin::AddTrayIcon(callbacks);
}

void DestroyUI(void) {
  Application::Exit();
}