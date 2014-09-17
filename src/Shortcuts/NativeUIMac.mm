#import "ApplicationDelegate.h"

#include "NativeUI.h"
#include "Shortcuts.h"

#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>
#include <autowiring/autowiring.h>

void NativeUI::ShowUI() {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}

void NativeUI::DestroyUI() {
}

void NativeUI::ConfigChanged(const std::string& config, const json11::Json& value) {
  @autoreleasepool {
    ApplicationDelegate* applicationDelegate = (ApplicationDelegate*)[[NSApplication sharedApplication] delegate];
    MenubarController* menubarController = [applicationDelegate menubarController];
    if (menubarController) {
      NSString* item = [NSString stringWithUTF8String:config.c_str()];
      [menubarController configChanged:item state:(BOOL)value.bool_value()];
    }
  }
}
