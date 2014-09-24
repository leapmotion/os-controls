#import "ApplicationDelegate.h"

#include "NativeUI.h"
#include "Shortcuts.h"

#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>
#include <Foundation/NSUserNotification.h>
#include <autowiring/autowiring.h>

void NativeUI::ShowUI() {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}

void NativeUI::DestroyUI() {
}

void NativeUI::ShowToolbarMessage(const char* title, const char* message) {
  //TODO - Get reviewed by someone who knows absolutely anything about Objective-C
  @autoreleasepool {
    NSUserNotification *notification = [[NSUserNotification alloc] init];
    notification.title = [NSString stringWithUTF8String:title];
    notification.informativeText = [NSString stringWithUTF8String:message];
    notification.soundName = NSUserNotificationDefaultSoundName;

    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
  }
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
