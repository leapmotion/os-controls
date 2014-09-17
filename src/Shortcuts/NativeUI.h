#pragma once
#include "autowiring/ContextMember.h"
#include "utility/ConfigEvent.h"

struct NativeUI:
  public ContextMember,
  public ConfigEvent
{
  /// <summary>
  /// Registers the system tray icon with the system to allow user interaction
  /// </summary>
  void ShowUI(void);

  /// <summary>
  /// Causes any previously displayed notification area UI to be destroyed
  /// </summary>
  /// <remarks>
  /// This operation is permanent and is called before application termination.  This method is idempotent.
  /// </remarks>
  void DestroyUI(void);

  /// <summary>
  /// Invoked by the UI when the user has indicated that we should quit
  /// </summary>
  void OnQuit(void);

  /// <summary>
  /// Invoked when the configuration dialog is presented
  /// </summary>
  void OnConfigUiVisible(void);

  /// <summary>
  /// Invoked when the configuration dialog is hidden
  /// </summary>
  /// <param name="bCancelled">True if the configuration dialog was dismissed via a "cancel" behavior</param>
  void OnConfigUiHidden(bool bCancelled);

  /// <summary>
  /// Invoked when a user changes a config setting from the UI.
  /// </summary>
  void OnSettingChanged(const std::string& var, bool state);

  /// <summary>
  /// Invoked when the user has requested that HTML help be displayed
  /// </summary>
  /// <param name="helpWith">A context clue used to indicate what the user specifically wants help with, may be nullptr</param>
  void OnShowHtmlHelp(const char* helpWith);

  /// <summary>
  /// Invoked when a config setting is altered from the native code.
  /// </summary>
  void ConfigChanged(const std::string& config, const json11::Json& value) override;
};