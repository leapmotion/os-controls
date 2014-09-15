#include "NativeUIWin.h"

using namespace oscontrols;

void ShowUI(void) {
  NativeUIWin^ nui = gcnew NativeUIWin();
  nui->Visible = true;
}
