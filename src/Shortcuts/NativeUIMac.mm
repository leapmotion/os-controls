#import "NativeUI.h"
#import "Shortcuts.h"
#import "ApplicationDelegate.h"
#import "utility/Config.h"

#import <AppKit/NSApplication.h>
#import <AppKit/NSNibLoading.h>
#import <Foundation/Foundation.h>
#import <autowiring/autowiring.h>

NativeUI::NativeUI() {
  // Set the primary config file
  std::string cfgPath = "./";
  @autoreleasepool {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    const char* c_str = [[paths objectAtIndex:0] UTF8String];
    if (c_str) {
      cfgPath = std::string(c_str);
      cfgPath += "/Leap Motion/";
    }
  }
  cfgPath += "Shortcuts.json";

  AutowiredFast<Config> cfg;
  cfg->SetPrimaryFile(cfgPath);
  cfg->RebroadcastConfig();

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

void NativeUI::ConfigChanged(const std::string& config, const json11::Json& value) {
  // Nothing to do on Mac (handled in PreferencePane)
}
