#import "MenubarController.h"
#import "NativeUI.h"

#import <autowiring/autowiring.h>

@implementation MenubarController

- (id)init
{
  self = [super init];
  if (self != nil) {
    _statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:24] retain];
    [_statusItem setImage:[NSImage imageNamed:@"TrayIcon"]];
    [_statusItem setAlternateImage:[NSImage imageNamed:@"TrayIconInverted"]];
    [_statusItem setHighlightMode:YES];

    NSMenu* statusMenu = [[NSMenu alloc] initWithTitle:@""];
    NSMenuItem* helpMenu = [[NSMenuItem alloc] initWithTitle:@"Help..."
                                                      action:@selector(onHelp:)
                                               keyEquivalent:@""];
    NSMenuItem* prefMenu = [[NSMenuItem alloc] initWithTitle:@"Preferences..."
                                                      action:@selector(onPreferences:)
                                               keyEquivalent:@""];
    NSMenuItem* quitMenu = [[NSMenuItem alloc] initWithTitle:@"Quit"
                                                      action:@selector(onQuit:)
                                               keyEquivalent:@""];
    [helpMenu setTarget:self];
    [prefMenu setTarget:self];
    [quitMenu setTarget:self];

    [statusMenu addItem:helpMenu];
    [statusMenu addItem:prefMenu];
    [statusMenu addItem:[NSMenuItem separatorItem]];
    [statusMenu addItem:quitMenu];

    [_statusItem setMenu:statusMenu];
  }
  return self;
}

- (void)onHelp:(id)sender
{
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnShowHtmlHelp("main");
  }
}

- (void)onPreferences:(id)sender
{
  @autoreleasepool {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSPreferencePanesDirectory, NSUserDomainMask, YES);
    NSString* userPrefPanePath = [paths objectAtIndex:0];
    // If we can't get the directory to the user's preference pane directory, give up
    if (userPrefPanePath == nil) {
      return;
    }
    [[NSWorkspace sharedWorkspace] openFile:[userPrefPanePath stringByAppendingString:@"/Shortcuts.prefPane"]];
  }
}

- (void)onQuit:(id)sender
{
  [[NSStatusBar systemStatusBar] removeStatusItem:_statusItem];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnQuit();
  } else { // Failsafe in case we fail to get the NativeUI instance
    [[NSApplication sharedApplication] terminate:nil];
  }
}

@end
