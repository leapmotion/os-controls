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
  // Hide ourselves from the Dock. Unfortunately, this causes our application
  // to hide as well. See our workaround for that problem in the
  // applicationDidHide: method below
  [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];

  _menubarController = [[MenubarController alloc] init];
  _isInitialized = YES;

  // Load config settings
  Autowired<Config> config;
  config.NotifyWhenAutowired([] {
    AutowiredFast<Config> cfg;
    if (cfg) {
      std::string path = "./";
      @autoreleasepool {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString *applicationSupportDirectory = [paths objectAtIndex:0];
        const char* c_str = [applicationSupportDirectory UTF8String];
        if (c_str) {
          path = std::string(c_str);
          path += "/Leap Motion/";
        }
      }
      path += "Shortcuts.json";
      cfg->SetPrimaryFile(path);
      cfg->RebroadcastConfig();
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
