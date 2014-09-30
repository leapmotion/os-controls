#import <Cocoa/Cocoa.h>

@interface MenubarController : NSObject {
@private
  NSStatusItem* _statusItem;
}

- (void)onPause:(id)sender;
- (void)onHelp:(id)sender;
- (void)onPreferences:(id)sender;
- (void)onQuit:(id)sender;

@end
