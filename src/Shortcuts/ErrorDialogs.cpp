#include "stdafx.h"
#include "ErrorDialogs.h"
#include "NativeUI.h"
#include "Leap.h"

ErrorDialogs::ErrorDialogs()
{
  *this += std::chrono::seconds(5), [this] { RaiseErrorMessages(); };
}

void ErrorDialogs::RaiseErrorMessages()
{
  AutowiredFast<Leap::Controller> controller;
  AutowiredFast<NativeUI> ui;
  if (!controller || !ui)
    return;

  if (!controller->isServiceConnected()) {
    ui->ShowToolbarMessage("Service not running",
      "Leap Motion Service not enabled.\n"
      "Launch the \"Leap Motion\" service to continue using Shortcuts.");
  }
  else if (!controller->isConnected()) {
    bool isEmbedded = false;
    for (auto device : controller->devices()){
      if (device.isEmbedded()){
        isEmbedded = true;
        break;
      }
    }

    ui->ShowToolbarMessage("Leap Motion Controller disconnected",
      "Leap Motion Controller not detected.\n"
      "Plug in a Leap Motion Controller to continue using Shortcuts.");
  }
  else if ((controller->policyFlags() & Leap::Controller::POLICY_BACKGROUND_FRAMES) == 0) {
    ui->ShowToolbarMessage("Background apps not enabled",
      "Leap Motion background app not enabled.\n"
      "Allow background apps in the Leap Motion Control Panel to continue using Shortcuts.");
  }
}
