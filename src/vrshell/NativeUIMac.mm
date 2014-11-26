#import "NativeUI.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSNibLoading.h>
#import <Foundation/Foundation.h>
#import <autowiring/autowiring.h>

NativeUI::NativeUI() {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}

void NativeUI::ShowUI() {
  [[NSApp delegate] addStatusItem];
}

void NativeUI::DestroyUI() {
}

void NativeUI::ShowToolbarMessage(const char* title, const char* message) {
  @autoreleasepool {
    NSUserNotification* notification = [[NSUserNotification alloc] init];
    notification.title = [NSString stringWithUTF8String:title];
    notification.informativeText = [NSString stringWithUTF8String:message];
    notification.soundName = NSUserNotificationDefaultSoundName;

    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
  }
}
