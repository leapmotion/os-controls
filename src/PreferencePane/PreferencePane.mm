#import "PreferencePane.h"

#import <Cocoa/Cocoa.h>

@implementation ComLeapMotionShortcutsPreferencePane

- (id)initWithBundle:(NSBundle *)bundle
{
  if (( self = [super initWithBundle:bundle] ) != nil) {
    appID = CFSTR("com.leapmotion.Shortcuts");
  }
  return self;
}

- (void)mainViewDidLoad
{
}

@end
