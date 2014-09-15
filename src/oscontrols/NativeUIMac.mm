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
