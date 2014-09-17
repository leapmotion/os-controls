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
    _mediaControlsMenu = [[NSMenuItem alloc] initWithTitle:@"Enable Media Controls"
                                                    action:@selector(onMediaControls:)
                                             keyEquivalent:@""];
    _windowSelectionMenu = [[NSMenuItem alloc] initWithTitle:@"Enable Window Selection"
                                                      action:@selector(onWindowSelection:)
                                               keyEquivalent:@""];
    _scrollingMenu = [[NSMenuItem alloc] initWithTitle:@"Enable Scrolling"
                                                action:@selector(onScrolling:)
                                         keyEquivalent:@""];
    NSMenuItem* helpMenu = [[NSMenuItem alloc] initWithTitle:@"Help..."
                                                      action:@selector(onHelp:)
                                               keyEquivalent:@""];
    NSMenuItem* quitMenu = [[NSMenuItem alloc] initWithTitle:@"Quit"
                                                      action:@selector(onQuit:)
                                               keyEquivalent:@""];
    [_mediaControlsMenu setState:NSOnState];
    [_windowSelectionMenu setState:NSOnState];
    [_scrollingMenu setState:NSOnState];

    [_mediaControlsMenu setTarget:self];
    [_windowSelectionMenu setTarget:self];
    [_scrollingMenu setTarget:self];
    [helpMenu setTarget:self];
    [quitMenu setTarget:self];

    [statusMenu addItem:helpMenu];
    [statusMenu addItem:[NSMenuItem separatorItem]];
    [statusMenu addItem:_mediaControlsMenu];
    [statusMenu addItem:_windowSelectionMenu];
    [statusMenu addItem:_scrollingMenu];
    [statusMenu addItem:[NSMenuItem separatorItem]];
    [statusMenu addItem:quitMenu];

    [_statusItem setMenu:statusMenu];
  }
  return self;
}

- (void)configChanged:(NSString*)name state:(BOOL)state
{
  if ([name isEqualToString:@"enableMedia"]) {
    [_mediaControlsMenu setState:(state ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"enableWindowSelection"]) {
    [_windowSelectionMenu setState:(state ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"enableScroll"]) {
    [_scrollingMenu setState:(state ? NSOnState : NSOffState)];
  }
}

- (void)onMediaControls:(id)sender
{
  NSInteger state = [_mediaControlsMenu state];
  if (state == NSOffState) {
    state = NSOnState;
  } else if (state == NSOnState) {
    state = NSOffState;
  } else {
    return;
  }
  [_mediaControlsMenu setState:state];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnSettingChanged("enableMedia", (state == NSOnState));
  }
}

- (void)onWindowSelection:(id)sender
{
  NSInteger state = [_windowSelectionMenu state];
  if (state == NSOffState) {
    state = NSOnState;
  } else if (state == NSOnState) {
    state = NSOffState;
  } else {
    return;
  }
  [_windowSelectionMenu setState:state];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnSettingChanged("enableWindowSelection", (state == NSOnState));
  }
}

- (void)onScrolling:(id)sender
{
  NSInteger state = [_scrollingMenu state];
  if (state == NSOffState) {
    state = NSOnState;
  } else if (state == NSOnState) {
    state = NSOffState;
  } else {
    return;
  }
  [_scrollingMenu setState:state];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnSettingChanged("enableScroll", (state == NSOnState));
  }
}

- (void)onHelp:(id)sender
{
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnShowHtmlHelp("main");
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
