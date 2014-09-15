#include "stdafx.h"
#include "NativeUI.h"
#include "oscontrols.h"

void NativeUI::OnQuit() {
  GetContext()->SignalShutdown(false);
}

void NativeUI::OnConfigUiVisible() {
}

void NativeUI::OnConfigUiHidden(bool) {
}
