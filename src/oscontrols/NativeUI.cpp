#include "NativeUI.h"

using namespace oscontrols;

void ShowUI(void) {
  NativeUI^ nui = gcnew NativeUI();
  nui->Visible = true;
}