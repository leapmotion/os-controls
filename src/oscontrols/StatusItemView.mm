#import "StatusItemView.h"
#import <cmath>

#import <iostream>

@implementation StatusItemView

@synthesize statusItem;
@synthesize image;

- (id)initWithStatusItem:(NSStatusItem*)newStatusItem
{
  CGFloat itemWidth = [newStatusItem length];
  CGFloat itemHeight = [[NSStatusBar systemStatusBar] thickness];
  NSRect itemRect = NSMakeRect(0.0f, 0.0f, itemWidth, itemHeight);
  self = [super initWithFrame:itemRect];

  if (self != nil) {
    _statusItem = [newStatusItem retain];
    _statusItem.view = self;
    _image = nil;
  }
  return self;
}

- (void)dealloc
{
  [_image release];
  [_statusItem release];
  [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect
{
  [_statusItem drawStatusBarBackgroundInRect:dirtyRect withHighlight:NO];

  NSImage* icon = _image;
  NSSize iconSize = [icon size];
  NSRect bounds = self.bounds;
  CGFloat iconX = std::round((NSWidth(bounds) - iconSize.width)/2.0f);
  CGFloat iconY = std::round((NSHeight(bounds) - iconSize.height)/2.0f);
  NSPoint iconPoint = NSMakePoint(iconX, iconY);

  [icon drawAtPoint:iconPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}

- (void)setImage:(NSImage*)newImage
{
  if (_image != newImage) {
    NSImage* oldImage = image;
    _image = [newImage retain];
    [oldImage release];
    [self setNeedsDisplay:YES];
  }
}

@end
