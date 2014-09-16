#include "stdafx.h"
#include "HtmlPageLauncherWin.h"

HtmlPageLauncherMac::HtmlPageLauncherMac(void)
{
}

HtmlPageLauncherMac::~HtmlPageLauncherMac(void)
{
}

HtmlPageLauncher* HtmlPageLauncher::New(void) {
  return new HtmlPageLauncherMac;
}

void HtmlPageLauncherWin::LaunchPage(const char* url) {

}