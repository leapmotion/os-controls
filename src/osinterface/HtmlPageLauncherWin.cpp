#include "stdafx.h"
#include "HtmlPageLauncherWin.h"

HtmlPageLauncherWin::HtmlPageLauncherWin(void)
{
}

HtmlPageLauncherWin::~HtmlPageLauncherWin(void)
{
}

HtmlPageLauncher* HtmlPageLauncher::New(void) {
  return new HtmlPageLauncherWin;
}

void HtmlPageLauncherWin::LaunchPage(const char* url) {
  SHELLEXECUTEINFOA info = {};
  info.cbSize = sizeof(info);
  info.fMask = 0;
  info.hwnd = nullptr;
  info.lpVerb = nullptr;
  info.lpFile = url;
  info.lpParameters = nullptr;
  info.lpDirectory = nullptr;
  info.nShow = SW_SHOW;
  info.hInstApp = nullptr;
  info.lpIDList = nullptr;
  info.lpClass = nullptr;
  info.hkeyClass = nullptr;
  info.dwHotKey = 0;

  ShellExecuteExA(&info);
}