#import "PreferencePane.h"
#import "utility/Config.h"
#import "../Shortcuts/Version.h"

#import <Cocoa/Cocoa.h>
#import <autowiring/autowiring.h>

class ConfigHandler:
  public ContextMember,
  public ConfigEvent
{
  public:
    ConfigHandler(ComLeapMotionShortcutsPreferencePane* preferencePane) : m_preferencePane(preferencePane) {}

    void ConfigChanged(const std::string& config, const json11::Json& value) override {
      @autoreleasepool {
        [m_preferencePane onChange:[NSString stringWithUTF8String:config.c_str()] withBool:(BOOL)value.bool_value()];
      }
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
  std::string path = "./";
  @autoreleasepool {
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
  }
}

- (IBAction)enableMediaControlsCheckbox:(id)sender
{
  AutowiredFast<Config> cfg;
  if (cfg) {
    cfg->Set("enableMedia", ([sender state] == NSOnState));
  }
}

- (IBAction)enableApplicationSwitcherCheckbox:(id)sender
{
  AutowiredFast<Config> cfg;
  if (cfg) {
    cfg->Set("enableWindowSelection", ([sender state] == NSOnState));
  }
}

- (IBAction)enableScrollingCheckbox:(id)sender
{
  AutowiredFast<Config> cfg;
  if (cfg) {
    cfg->Set("enableScroll", ([sender state] == NSOnState));
  }
}

- (IBAction)enableAutoStart:(id)sender
{
}

- (IBAction)launchShortcuts:(id)sender
{
  @autoreleasepool {
    [[NSWorkspace sharedWorkspace] openFile:[@"~/Applications/AirspaceApps/Shortcuts.app" stringByExpandingTildeInPath]];
  }
}

@end
