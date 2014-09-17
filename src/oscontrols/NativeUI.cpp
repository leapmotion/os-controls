#include "stdafx.h"
#include "NativeUI.h"
#include "oscontrols.h"
#include "osinterface/HtmlPageLauncher.h"
#include "utility/Config.h"

#include <autowiring/Autowired.h>

void NativeUI::OnQuit() {
  GetContext()->SignalShutdown(false);
}

void NativeUI::OnConfigUiVisible() {
}

void NativeUI::OnConfigUiHidden(bool) {
}

void NativeUI::OnShowHtmlHelp(const char* helpWith) {
  AutowiredFast<HtmlPageLauncher> hpl;
  if(hpl)
    hpl->LaunchPage("http://handcontrols.leapmotion.com/help.html");
}

void NativeUI::OnSettingChanged(const std::string& var, bool state)
{
  AutowiredFast<Config> config;
  if (!config)
    return;

  config->Set(var, state);
}
