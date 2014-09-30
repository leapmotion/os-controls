#import <PreferencePanes/PreferencePanes.h>

@interface ComLeapMotionShortcutsPreferencePane : NSPreferencePane {
  CFStringRef appID;
}
@property (unsafe_unretained) IBOutlet NSTextField *versionLabel;
@property (unsafe_unretained) IBOutlet NSButtonCell *launchShortcutsButton;
@property (unsafe_unretained) IBOutlet NSButton *mediaControlsCheckbox;
@property (unsafe_unretained) IBOutlet NSButton *applicationSwitcherCheckbox;
@property (unsafe_unretained) IBOutlet NSButton *scrollingCheckbox;
@property (unsafe_unretained) IBOutlet NSButton *autoStartCheckbox;
@property (unsafe_unretained) IBOutlet NSButton *scrollDirectionCheckbox;
@property (unsafe_unretained) IBOutlet NSSlider *sensitivityBar;

- (id)initWithBundle:(NSBundle *)bundle;
- (void)mainViewDidLoad;
- (void)onChange:(NSString *)name withBool:(BOOL)value;
- (void)onChange:(NSString *)name withNumber:(double)value;
- (void)onChangeAutoLaunch:(BOOL)value;

- (IBAction)launchShortcuts:(id)sender;
- (IBAction)enableMediaControlsCheckbox:(id)sender;
- (IBAction)enableApplicationSwitcherCheckbox:(id)sender;
- (IBAction)enableScrollingCheckbox:(id)sender;
- (IBAction)enableAutoStart:(id)sender;
- (IBAction)enableReverseScroll:(id)sender;
- (IBAction)onUserSensitivtyChanged:(id)sender;

@end
