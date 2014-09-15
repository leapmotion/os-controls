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
    NSMenuItem* prefMenu = [[NSMenuItem alloc] initWithTitle:@"Preferences..." action:@selector(onPreferences:) keyEquivalent:@""];
    NSMenuItem* quitMenu = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(onQuit:) keyEquivalent:@""];
    [prefMenu setTarget:self];
    [quitMenu setTarget:self];
//  [statusMenu addItem:prefMenu]; //  Disable the until we have something to actually do
    [statusMenu addItem:quitMenu];
    [_statusItem setMenu:statusMenu];
  }
  return self;
}

- (void)dealloc
{
  [_statusItem release];
  [super dealloc];
}

- (void)onPreferences:(id)sender
{
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
