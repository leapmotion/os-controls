#include "NativeUI.h"
#include "oscontrols.h"

#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>
#include <autowiring/autowiring.h>

void NativeUI::ShowUI() {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}

void NativeUI::DestroyUI() {
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
