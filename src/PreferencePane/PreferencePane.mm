#import "PreferencePane.h"
#import "utility/Config.h"
#import "utility/AutoLaunch.h"
#import "../Shortcuts/Version.h"

#import <Cocoa/Cocoa.h>
#import <autowiring/autowiring.h>

class ConfigHandler:
  public ContextMember,
  public ConfigEvent,
  public AutoLaunchEvent
{
  public:
    ConfigHandler(ComLeapMotionShortcutsPreferencePane* preferencePane) : m_preferencePane(preferencePane) {}

    void ConfigChanged(const std::string& config, const json11::Json& value) override {
      @autoreleasepool {
        if (value.is_bool()) {
          [m_preferencePane onChange:[NSString stringWithUTF8String:config.c_str()] withBool:(BOOL)value.bool_value()];
        } else if (value.is_number()) {
          [m_preferencePane onChange:[NSString stringWithUTF8String:config.c_str()] withNumber:value.number_value()];
        }
      }
    }

    void OnAutoLaunchChanged(bool newValue) {
      [m_preferencePane onChangeAutoLaunch:(BOOL)newValue];
    }

  private:
    ComLeapMotionShortcutsPreferencePane* m_preferencePane;
};

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
  @autoreleasepool {
    [_versionLabel setStringValue:[NSString stringWithUTF8String:Shortcuts_VERSION]];
  }
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoConstruct<ConfigHandler> cfgHandler(self);
  AutoRequired<Config> cfg;
  AutoRequired<AutoLaunch> autoLaunch;
  std::string path = "./";
  @autoreleasepool {
    [_autoStartCheckbox setState:(autoLaunch->IsAutoLaunch() ? NSOnState : NSOffState)];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *applicationSupportDirectory = [paths objectAtIndex:0];
    const char* c_str = [applicationSupportDirectory UTF8String];
    if (c_str) {
      path = std::string(c_str);
      path += "/Leap Motion/";
    }
  }
  path += "Shortcuts.json";
  cfg->SetPrimaryFile(path);
}

- (void)onChange:(NSString *)name withBool:(BOOL)value
{
  if ([name isEqualToString:@"enableMedia"]) {
    [_mediaControlsCheckbox setState:(value ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"enableWindowSelection"]) {
    [_applicationSwitcherCheckbox setState:(value ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"enableScroll"]) {
    [_scrollingCheckbox setState:(value ? NSOnState : NSOffState)];
  } else if ([name isEqualToString:@"reverseScroll"]) {
    [_scrollDirectionCheckbox setState:(value ? NSOnState : NSOffState)];
  }
}

- (void)onChange:(NSString *)name withNumber:(double)value
{
  if ([name isEqualToString:@"scrollSensitivity"]) {
    [_sensitivityBar setDoubleValue:value];
  }
}

- (void)onChangeAutoLaunch:(BOOL)value
{
  [_autoStartCheckbox setState:(value ? NSOnState : NSOffState)];
}

- (IBAction)enableMediaControlsCheckbox:(id)sender
{
  AutowiredFast<Config> cfg;
  if (!cfg) {
    return;
  }
  cfg->Set("enableMedia", ([sender state] == NSOnState));
}

- (IBAction)enableApplicationSwitcherCheckbox:(id)sender
{
  AutowiredFast<Config> cfg;
  if (!cfg) {
    return;
  }
  cfg->Set("enableWindowSelection", ([sender state] == NSOnState));
}

- (IBAction)enableScrollingCheckbox:(id)sender
{
  AutowiredFast<Config> cfg;
  if (!cfg) {
    return;
  }
  cfg->Set("enableScroll", ([sender state] == NSOnState));
}

- (IBAction)enableAutoStart:(id)sender
{
  AutowiredFast<AutoLaunch> autoLaunch;
  if (!autoLaunch) {
    return;
  }
  autoLaunch->SetAutoLaunch([sender state] == NSOnState);
}

- (IBAction)enableReverseScroll:(id)sender
{
  AutowiredFast<Config> cfg;
  if (!cfg) {
    return;
  }
  cfg->Set("reverseScroll", ([sender state] == NSOnState));
}

- (IBAction)onUserSensitivtyChanged:(id)sender
{
  AutowiredFast<Config> cfg;
  if (!cfg) {
    return;
  }
  cfg->Set("scrollSensitivity", [sender doubleValue]);
}

- (IBAction)launchShortcuts:(id)sender
{
  @autoreleasepool {
    NSString* path = [@"~/Applications/AirspaceApps/Shortcuts.app" stringByExpandingTildeInPath];
    NSString* shortcutsPrefPanePath = [[self bundle] bundlePath];
    NSString* expectedEnd = @"/Shortcuts.app/Contents/MacOS/Shortcuts.prefPane";
    NSUInteger length = shortcutsPrefPanePath.length;

    if (length >= expectedEnd.length) {
      path = [shortcutsPrefPanePath substringToIndex:(length - expectedEnd.length + 14)]; // (+ 14 => "/Shortcuts.app")
    }
    NSString* resolvedPath = [[NSFileManager defaultManager] destinationOfSymbolicLinkAtPath:path error:nil];
    if (resolvedPath) {
      path = resolvedPath;
    }
    [[NSWorkspace sharedWorkspace] openFile:path];
  }
}

@end
