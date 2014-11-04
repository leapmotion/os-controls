#include "NativeUI.h"
#include "NativeUIWin.h"

using namespace VRShell;

NativeUI::NativeUI() {

}

void NativeUI::ShowUI() {
  NativeUIWin::AddTrayIcon(*this);
}

void NativeUI::DestroyUI() {
  Application::Exit();
}

void NativeUI::ShowToolbarMessage(const char* title, const char* message) {
  NativeUIWin::ShowToolbarMessage(title, message);
}

