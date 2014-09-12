#import <Cocoa/Cocoa.h>

@interface StatusItemView : NSView {
@private
  NSImage* _image;
  NSStatusItem* _statusItem;
}

- (id)initWithStatusItem:(NSStatusItem*)newStatusItem;

@property (nonatomic, readonly) NSStatusItem* statusItem;
@property (nonatomic, retain) NSImage* image;

@end
