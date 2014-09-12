#import <Cocoa/Cocoa.h>

@class StatusItemView;

@interface MenubarController : NSObject {
@private
  StatusItemView* _statusItemView;
}

@property (nonatomic, readonly) StatusItemView* statusItemView;

@end
