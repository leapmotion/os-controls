#include "stdafx.h"
#include "HtmlPageLauncherMac.h"

#import <AppKit/NSWorkspace.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSString.h>

HtmlPageLauncherMac::HtmlPageLauncherMac(void)
{
}

HtmlPageLauncherMac::~HtmlPageLauncherMac(void)
{
}

HtmlPageLauncher* HtmlPageLauncher::New(void) {
  return new HtmlPageLauncherMac;
}

void HtmlPageLauncherMac::LaunchPage(const char* url) {
  @autoreleasepool {
    NSString* urlString = [NSString stringWithUTF8String:url];
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:urlString]];
  }
}
