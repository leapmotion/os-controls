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
    _mediaControls = [[NSMenuItem alloc] initWithTitle:@"Enable Media Controls"
                                                action:@selector(onMediaControls:)
                                         keyEquivalent:@""];
    _windowSelection = [[NSMenuItem alloc] initWithTitle:@"Enable Window Selection"
                                                  action:@selector(onWindowSelection:)
                                           keyEquivalent:@""];
    _scrolling = [[NSMenuItem alloc] initWithTitle:@"Enable Scrolling"
                                            action:@selector(onScrolling:)
                                     keyEquivalent:@""];
    NSMenuItem* quitMenu = [[NSMenuItem alloc] initWithTitle:@"Quit"
                                                      action:@selector(onQuit:)
                                               keyEquivalent:@""];
    [_mediaControls setState:NSOnState];
    [_windowSelection setState:NSOnState];
    [_scrolling setState:NSOnState];

    [_mediaControls setTarget:self];
    [_windowSelection setTarget:self];
    [_scrolling setTarget:self];
    [quitMenu setTarget:self];

    [statusMenu addItem:_mediaControls];
    [statusMenu addItem:_windowSelection];
    [statusMenu addItem:_scrolling];
    [statusMenu addItem:quitMenu];

    [_statusItem setMenu:statusMenu];
  }
  return self;
}

- (void)dealloc
{
  [_statusItem release];
  [_scrolling release];
  [_windowSelection release];
  [_mediaControls release];
  [super dealloc];
}

- (void)configChanged:(NSString*)name state:(BOOL)state
{
  if ([name isEqualToString:@"enableMedia"]) {
    [_mediaControls setState:(state ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"enableWindowSelection"]) {
    [_windowSelection setState:(state ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"enableScroll"]) {
    [_scrolling setState:(state ? NSOnState : NSOffState)];
  }
}

- (void)onMediaControls:(id)sender
{
  NSInteger state = [_mediaControls state];
  if (state == NSOffState) {
    state = NSOnState;
  } else if (state == NSOnState) {
    state = NSOffState;
  } else {
    return;
  }
  [_mediaControls setState:state];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnSettingChanged("enableMedia", (state == NSOnState));
  }
}

- (void)onWindowSelection:(id)sender
{
  NSInteger state = [_windowSelection state];
  if (state == NSOffState) {
    state = NSOnState;
  } else if (state == NSOnState) {
    state = NSOffState;
  } else {
    return;
  }
  [_windowSelection setState:state];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnSettingChanged("enableWindowSelection", (state == NSOnState));
  }
}

- (void)onScrolling:(id)sender
{
  NSInteger state = [_scrolling state];
  if (state == NSOffState) {
    state = NSOnState;
  } else if (state == NSOnState) {
    state = NSOffState;
  } else {
    return;
  }
  [_scrolling setState:state];
  AutowiredFast<NativeUI> nativeUI;
  if (nativeUI) {
    nativeUI->OnSettingChanged("enableScroll", (state == NSOnState));
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
