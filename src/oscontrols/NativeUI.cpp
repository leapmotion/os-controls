#include "NativeUI.h"

#if __APPLE__
#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>
#elif defined(_MSC_VER) && _MANAGED
using namespace oscontrols;
#endif

void ShowUI(void) {
#if __APPLE__
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
#elif defined(_MSC_VER) && _MANAGED
  NativeUI^ nui = gcnew NativeUI();
  nui->Visible = true;
#endif
}
