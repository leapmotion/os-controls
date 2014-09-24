#import <PreferencePanes/PreferencePanes.h>

@interface ComLeapMotionShortcutsPreferencePane : NSPreferencePane {
  CFStringRef appID;
}

- (id)initWithBundle:(NSBundle *)bundle;
- (void)mainViewDidLoad;

@end
