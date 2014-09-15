#pragma once

struct NativeCallbacks {
  /// <summary>
  /// Invoked by the UI when the user has indicated that we should quit
  /// </summary>
  void(*OnQuit)();

  /// <summary>
  /// Invoked when the configuration dialog is presented
  /// </summary>
  void(*OnConfigUiVisible)(void);

  /// <summary>
  /// Invoked when the configuration dialog is hidden
  /// </summary>
  /// <param name="bCancelled">True if the configuration dialog was dismissed via a "cancel" behavior</param>
  void(*OnConfigUiHidden)(bool bCancelled);
};

/// <summary>
/// Registers the system tray icon with the system to allow user interaction
/// </summary>
void ShowUI(const NativeCallbacks& callbacks);

/// <summary>
/// Causes any previously displayed notification area UI to be destroyed
/// </summary>
/// <remarks>
/// This operation is permanent and is called before application termination.  This method is idempotent.
/// </remarks>
void DestroyUI(void);