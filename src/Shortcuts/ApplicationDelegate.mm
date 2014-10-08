#import "ApplicationDelegate.h"
#import "utility/Config.h"

#import <autowiring/autowiring.h>

@implementation ApplicationDelegate

@synthesize menubarController = _menubarController;

- (id)init
{
  self = [super init];
  if (self != nil) {
    // Defer initialization until applicationDidFinishLaunching:
    _menubarController = nil;
    _isInitialized = NO;
    _wasHidden = NO;
  }
  return self;
}

- (void)dealloc
{
  [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
  if (_isInitialized) {
    return;
  }
  _isInitialized = YES;

  // Hide ourselves from the Dock. Unfortunately, this causes our application
  // to hide as well. See our workaround for that problem in the
  // applicationDidHide: method below
  NSApplication* app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyAccessory];
  if ([app activationPolicy] != NSApplicationActivationPolicyAccessory) {
    [app setActivationPolicy:NSApplicationActivationPolicyProhibited];
  }

  // Check and possibly update the link to the Shortcuts preference pane
  @autoreleasepool {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSPreferencePanesDirectory, NSUserDomainMask, YES);
    NSString* userPrefPanePath = [paths objectAtIndex:0];
    // If we can't get the directory to the user's preference pane directory, give up
    if (userPrefPanePath == nil) {
      return;
    }
    userPrefPanePath = [userPrefPanePath stringByAppendingString:@"/Shortcuts.prefPane"];

    NSBundle* bundle = [NSBundle mainBundle];
    NSString* shortcutsPrefPanePath = [bundle bundlePath];
    // If we can't get the path to this app bundle, give up
    if (shortcutsPrefPanePath == nil) {
      return;
    }
    shortcutsPrefPanePath = [shortcutsPrefPanePath stringByAppendingString:@"/Contents/MacOS/Shortcuts.prefPane"];
    NSString* shortcutsPrefPanePathExec =
      [shortcutsPrefPanePath stringByAppendingString:@"/Contents/MacOS/ShortcutsPreferences"];

    NSFileManager* fileManager = [NSFileManager defaultManager];
    // If we can't find the Shortcuts preferences executable, give up
    if (![fileManager fileExistsAtPath:shortcutsPrefPanePathExec]) {
      return;
    }

    // See whether or not the Shortcuts preference pane is pointing to the expected instance
    NSString* symbolicLinkPath = [fileManager destinationOfSymbolicLinkAtPath:userPrefPanePath error:nil];
    if (symbolicLinkPath && [shortcutsPrefPanePath compare:symbolicLinkPath] == NSOrderedSame) {
      return;
    }
    // Try to remove the old link (if it exists at all)
    [fileManager removeItemAtPath:userPrefPanePath error:nil];
    // Create a symbolic link to our Shortcuts preference pane app in the user's preference pane directory
    [fileManager createSymbolicLinkAtPath:userPrefPanePath withDestinationPath:shortcutsPrefPanePath error:nil];
  }
}

- (void)addStatusItem
{
  if (_menubarController) {
    return;
  }
  _menubarController = [[MenubarController alloc] init];
  // Load config settings
  AutoCurrentContext ctxt;
  ctxt->NotifyWhenAutowired<Config>([self] {
    AutowiredFast<Config> cfg;
    if (cfg) {
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
      cfg->SetPrimaryFile(cfgPath);
      cfg->RebroadcastConfig();

      if (cfg->Get<bool>("showHelpOnStart")) {
        [_menubarController onHelp:nil];
        cfg->Set("showHelpOnStart", false);
      }
    }
  });
}

- (void)applicationDidHide:(NSNotification*)aNotification
{
  // In the NativeWindow::MakeTransparent(), we call the -[NSWindow
  // setHidesOnDeactivate:NO] method, which tells the window not to hide when
  // the application deactivates. Unfortunately, that is too late, as we will
  // have already been deactivated by the time that is acknowledged by the
  // window manager. Because of our policy change earlier, the result is that
  // the window hide. The workaround is that the first time we are told to
  // hide, we will immediately ask to be unhidden.

  if (_wasHidden) {
    return;
  }
  [NSApp unhide:nil];
  _wasHidden = true;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
  [_menubarController release];
  _menubarController = nil;
  return NSTerminateNow;
}

@end
