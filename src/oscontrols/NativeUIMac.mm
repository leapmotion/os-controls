#include "NativeUI.h"
#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>

void ShowUI(const NativeCallbacks& callbacks) {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}
