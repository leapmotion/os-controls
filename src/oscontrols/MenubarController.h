#import <Cocoa/Cocoa.h>

@interface MenubarController : NSObject {
@private
  NSStatusItem* _statusItem;
  NSMenuItem* _mediaControls;
  NSMenuItem* _windowSelection;
  NSMenuItem* _scrolling;
}

- (void)configChanged:(NSString*)name state:(BOOL)state;

- (void)onMediaControls:(id)sender;
- (void)onWindowSelection:(id)sender;
- (void)onScrolling:(id)sender;
- (void)onQuit:(id)sender;

@end
