#import <Cocoa/Cocoa.h>

@interface MenubarController : NSObject {
@private
  NSStatusItem* _statusItem;
  NSMenuItem* _mediaControlsMenu;
  NSMenuItem* _windowSelectionMenu;
  NSMenuItem* _scrollingMenu;
}

- (void)configChanged:(NSString*)name state:(BOOL)state;

- (void)onMediaControls:(id)sender;
- (void)onWindowSelection:(id)sender;
- (void)onScrolling:(id)sender;
- (void)onHelp:(id)sender;
- (void)onQuit:(id)sender;

@end
