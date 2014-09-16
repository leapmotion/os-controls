#include "NativeUI.h"
#include "oscontrols.h"

#include <AppKit/NSApplication.h>
#include <AppKit/NSNibLoading.h>
#include <autowiring/autowiring.h>

void NativeUI::ShowUI() {
  [[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:[NSApplication sharedApplication] topLevelObjects:nil];
}

void NativeUI::DestroyUI() {
}

void NativeUI::ConfigChanged(const std::string& config, const json11::Json& value) {
  //TODO: Implementation
}