#include "NativeUI.h"
#include "NativeUIWin.h"

using namespace Shortcuts;

void NativeUI::ShowUI() {
  NativeUIWin::AddTrayIcon(*this);
}

void NativeUI::DestroyUI(void) {
  Application::Exit();
}

void NativeUI::ShowToolbarMessage(const char* title, const char* message) {
  NativeUIWin::ShowToolbarMessage(title, message);
}

void NativeUI::ConfigChanged(const std::string& config, const json11::Json& value) {
  if (value.is_bool())
    NativeUIWin::ConfigChanged(config, value.bool_value());
  else if (value.is_number())
    NativeUIWin::ConfigChanged(config, value.number_value());
}
