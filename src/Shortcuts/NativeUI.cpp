#include "stdafx.h"
#include "NativeUI.h"
#include "Shortcuts.h"
#include "osinterface/HtmlPageLauncher.h"
#include "osinterface/PauseInteractionEvent.h"
#include "utility/AutoLaunch.h"
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
    hpl->LaunchPage("http://shortcuts.leapmotion.com/help.html");
}

void NativeUI::RequestConfigs() {
  AutowiredFast<Config> config;
  if (!config)
    return;

  config->RebroadcastConfig();
}

void NativeUI::SetUserConfigFile(const std::string& file)
{
  AutoCurrentContext ctxt;
  ctxt->NotifyWhenAutowired<Config>([file]() {
    AutowiredFast<Config> cfg;
    cfg->SetPrimaryFile(file);
  });
}

void NativeUI::OnSettingChanged(const std::string& var, bool state)
{
  AutowiredFast<Config> config;
  if (!config)
    return;

  config->Set(var, state);
}

void NativeUI::OnSettingChanged(const std::string& var, double value)
{
  AutowiredFast<Config> config;
  if (!config)
    return;

  config->Set(var, value);
}

void NativeUI::OnStartupChanged(bool value) {
  AutowiredFast<AutoLaunch> launch;
  if (!launch)
    return;
  launch->SetAutoLaunch(value);
}

void NativeUI::OnPauseInteraction(bool value) {
  AutoFired<PauseInteractionEvent> toggleEvent;
  toggleEvent(&PauseInteractionEvent::PauseChanged)(value);
}

bool NativeUI::GetLaunchOnStartup() {
  AutowiredFast<AutoLaunch> launch;
  if (!launch)
    return false;
  return launch->IsAutoLaunch();
}