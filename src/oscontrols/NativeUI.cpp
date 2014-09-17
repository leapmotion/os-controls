#include "stdafx.h"
#include "NativeUI.h"
#include "oscontrols.h"
#include "utility/Config.h"

#include <autowiring/Autowired.h>

void NativeUI::OnQuit() {
  GetContext()->SignalShutdown(false);
}

void NativeUI::OnConfigUiVisible() {
}

void NativeUI::OnConfigUiHidden(bool) {
}

void NativeUI::OnSettingChanged(const std::string& var, bool state)
{
  AutowiredFast<Config> config;
  if (!config)
    return;

  config->Set(var, state);
}
