#include "NativeUI.h"
#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>

void ShowUI(void) {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}
