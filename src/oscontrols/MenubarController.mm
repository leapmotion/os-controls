#import "MenubarController.h"
#import "StatusItemView.h"

@implementation MenubarController

@synthesize statusItemView = _statusItemView;

- (id)init
{
  self = [super init];
  if (self != nil) {
    NSStatusItem* statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:24];
    _statusItemView = [[StatusItemView alloc] initWithStatusItem:statusItem];
    _statusItemView.image = [NSImage imageNamed:@"TrayIcon"];
  }
  return self;
}

- (void)dealloc
{
  [[NSStatusBar systemStatusBar] removeStatusItem:[_statusItemView statusItem]];
  [_statusItemView release];
  [super dealloc];
}

@end
